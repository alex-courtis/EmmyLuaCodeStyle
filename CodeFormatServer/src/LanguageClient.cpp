#include "CodeFormatServer/LanguageClient.h"

#include "CodeService/LuaEditorConfig.h"
#include "CodeService/LuaFormatter.h"
#include "LuaParser/LuaParser.h"
#include "Util/format.h"

LanguageClient& LanguageClient::GetInstance()
{
	static LanguageClient instance;
	return instance;
}

LanguageClient::LanguageClient()
	: _defaultOptions(std::make_shared<LuaCodeStyleOptions>())
{
}

void LanguageClient::SetSession(std::shared_ptr<IOSession> session)
{
	_session = session;
}

std::shared_ptr<IOSession> LanguageClient::GetSession()
{
	return _session;
}

void LanguageClient::SendNotification(std::string_view method, std::shared_ptr<vscode::Serializable> param)
{
	auto json = nlohmann::json::object();
	json["jsonrpc"] = "2.0";
	json["method"] = method;
	json["params"] = param->Serialize();

	if (_session)
	{
		auto dumpResult = json.dump();
		std::string message = format("Content-Length:{}\r\n\r\n", dumpResult.size());

		message.append(dumpResult);
		_session->Send(std::move(message));
	}
}

void LanguageClient::CacheFile(std::string_view uri, std::shared_ptr<LuaParser> parser)
{
	std::filesystem::path path(uri);
	parser->SetFilename(path.filename().string());
	_parserMap[std::string(uri)] = parser;
}

void LanguageClient::ReleaseFile(std::string_view uri)
{
	auto it = _parserMap.find(uri);
	if (it != _parserMap.end())
	{
		_parserMap.erase(it);
	}

}

void LanguageClient::DiagnosticFile(std::string_view uri)
{
	auto it = _parserMap.find(uri);
	if (it == _parserMap.end())
	{
		return;
	}

	auto options = GetOptions(uri);
	if (!options->enable_check_codestyle)
	{
		auto vscodeDiagnosis = std::make_shared<vscode::PublishDiagnosticsParams>();
		vscodeDiagnosis->uri = uri;
		SendNotification("textDocument/publishDiagnostics", vscodeDiagnosis);
		return;
	}

	std::shared_ptr<LuaParser> parser = it->second;

	if (parser->HasError())
	{
		return;
	}

	LuaFormatter formatter(parser, *options);
	formatter.BuildFormattedElement();

	auto diagnosisInfos = formatter.GetDiagnosisInfos();

	auto vscodeDiagnosis = std::make_shared<vscode::PublishDiagnosticsParams>();
	vscodeDiagnosis->uri = uri;

	for (auto diagnosisInfo : diagnosisInfos)
	{
		auto& diagnosis = vscodeDiagnosis->diagnostics.emplace_back();
		diagnosis.message = diagnosisInfo.Message;
		diagnosis.range = vscode::Range(
			vscode::Position(
				diagnosisInfo.Range.Start.Line,
				diagnosisInfo.Range.Start.Character
			),
			vscode::Position(
				diagnosisInfo.Range.End.Line,
				diagnosisInfo.Range.End.Character
			));
		diagnosis.severity = vscode::DiagnosticSeverity::Warning;
	}

	SendNotification("textDocument/publishDiagnostics", vscodeDiagnosis);
}

std::shared_ptr<LuaParser> LanguageClient::GetFileParser(std::string_view uri)
{
	auto it = _parserMap.find(uri);
	if (it != _parserMap.end())
	{
		return it->second;
	}

	return nullptr;
}

void LanguageClient::Run()
{
	if (_session)
	{
		_session->Run();
	}
}

std::shared_ptr<LuaCodeStyleOptions> LanguageClient::GetOptions(std::string_view uri)
{
	std::size_t matchLength = 0;
	std::shared_ptr<LuaCodeStyleOptions> options = _defaultOptions;
	for (auto it = _editorConfigVector.begin(); it != _editorConfigVector.end(); it++)
	{
		if (uri.starts_with(it->first) && it->first.size() > matchLength)
		{
			matchLength = it->first.size();
			options = it->second->Generate(uri);
		}
	}

	return options;
}

void LanguageClient::UpdateOptions(std::string_view workspaceUri, std::string_view configPath)
{
	for (auto& pair : _editorConfigVector)
	{
		if (pair.first == workspaceUri)
		{
			pair.second = LuaEditorConfig::LoadFromFile(std::string(configPath));
			pair.second->SetWorkspace(workspaceUri);
			return;
		}
	}

	_editorConfigVector.push_back({
		std::string(workspaceUri),
		LuaEditorConfig::LoadFromFile(std::string(configPath))
	});
	_editorConfigVector.back().second->SetWorkspace(workspaceUri);
}

void LanguageClient::RemoveOptions(std::string_view workspaceUri)
{
	for (auto it = _editorConfigVector.begin(); it != _editorConfigVector.end(); it++)
	{
		if (it->first == workspaceUri)
		{
			_editorConfigVector.erase(it);
			break;
		}
	}
}

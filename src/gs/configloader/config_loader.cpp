#include <gs/configloader/config_loader.h>
#include <gs/system/log.h>
#include <tml/tml_parser.h>
#include <tml/tml_file_loader.h>
#include <cfg/cfg_include.h>
#include <cfg/cfg_template.h>
#include <cfg/cfg_translation.h>
#include <interpreter/interpreter.h>

bool gs::configloader::getConfigAsTree(const std::string& configFilename,
		cfg::NameValuePair &root)
{
#if 1
	cfg::TmlParser parser(configFilename);
	if (!parser.getAsTree(root)) {
		LOGE("Can't parse scene file. Error: %s\n",
				parser.getExtendedErrorMsg().c_str());
		return false;
	}
	cfg::Value& value = root.mValue;
	std::string errorMsg;
#else
	// this version would allowed includes (but more tests for default necessary)
	cfg::TmlFileLoader loader;
	cfg::Value value;
	std::string errorMsg;
	cfg::inc::TFileMap includedFiles;
	if (!cfg::inc::loadAndIncludeFiles(value, includedFiles, configFilename, loader,
			"include", false, false, false,
			errorMsg)) {
		LOGE("parse/includes for %s failed", configFilename.c_str());
		LOGE("error: %s", errorMsg.c_str());
		return false;
	}
	root.mName.setText(configFilename);
	root.mValue = value;
#endif

	cfg::cfgtemp::TemplateMap templateMap;
	if (!cfg::cfgtemp::addTemplates(templateMap, value, true, "template",
			errorMsg)) {
		LOGE("%s\n", errorMsg.c_str());
		return false;
	}
	if (!cfg::cfgtemp::useTemplates(templateMap, value,
			"use-template", true, false, errorMsg)) {
		LOGE("%s\n", errorMsg.c_str());
		return false;
	}


	if (!cfg::cfgtr::applyVariables(value, "variables",
			"$(", errorMsg)) {
		LOGE("apply variables FAILED");
		LOGE("%s", errorMsg.c_str());
		return false;
	}

	std::stringstream errMsg;
	if (cfg::interpreter::interpretAndReplace(value, false, true, true, true, errMsg) == -1) {
		LOGE("interpreter failed, err msg:\n");
		LOGE("%s", errMsg.str().c_str());
		return false;
	}

	return true;
}

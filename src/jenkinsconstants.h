#ifndef JENKINSCONSTANTS_H
#define JENKINSCONSTANTS_H


namespace Jenkins {
namespace Constants {


// QSettings keys for use by the "New Class" wizards.
const char * const JENKINS_SETTINGSGROUP = "Jenkins";
const char * const JENKINS_URL = "URL";
const char * const JENKINS_IGNORED = "Ignored";
const char * const JENKINS_REFRESH = "RefreshInterval";
const char * const JENKINS_USERNAME = "Username";
const char * const JENKINS_PASSWORD = "Password";

const char * const JENKINS_SETTINGS_ID = QT_TRANSLATE_NOOP("Jenkins", "Jenkins Options");
const char * const JENKINS_SETTINGS_CATEGORY = QT_TRANSLATE_NOOP("Jenkins", "Jenkins");


} // namespace Constants
} // namespace CppHelper

#endif // JENKINSCONSTANTS_H

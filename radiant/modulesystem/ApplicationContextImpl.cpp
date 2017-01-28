#include "ApplicationContextImpl.h"

#include <locale>
#include "string/string.h"
#include "debugging/debugging.h"
#include "itextstream.h"
#include "iregistry.h"
#include "os/path.h"
#include "os/dir.h"
#include "log/PopupErrorHandler.h"
#include "log/LogStream.h"

#include <boost/algorithm/string/predicate.hpp>

#if defined(WIN32)
#include <windows.h>
#endif

namespace radiant
{

/**
 * Return the application path of the current Radiant instance.
 */
std::string ApplicationContextImpl::getApplicationPath() const
{
	return _appPath;
}

std::string ApplicationContextImpl::getRuntimeDataPath() const
{
#if defined(POSIX) && defined (PKGDATADIR)
    return std::string(PKGDATADIR) + "/";
#else
    return getApplicationPath();
#endif
}

std::string ApplicationContextImpl::getSettingsPath() const
{
	return _settingsPath;
}

std::string ApplicationContextImpl::getBitmapsPath() const
{
	return getRuntimeDataPath() + "bitmaps/";
}

const ApplicationContext::ArgumentList&
ApplicationContextImpl::getCmdLineArgs() const
{
	return _cmdLineArgs;
}

std::ostream& ApplicationContextImpl::getOutputStream() const 
{
    return GlobalOutputStream().getStream();
}

std::ostream& ApplicationContextImpl::getWarningStream() const 
{
    return GlobalWarningStream().getStream();
}

std::ostream& ApplicationContextImpl::getErrorStream() const 
{
    return GlobalErrorStream().getStream();
}

std::mutex& ApplicationContextImpl::getStreamLock() const
{
    return applog::LogStream::GetStreamLock();
}

// ============== OS-Specific Implementations go here ===================

// ================ POSIX ====================
#if defined(POSIX)

#include <stdlib.h>
#include <pwd.h>
#include <unistd.h>

const char* LINK_NAME =
#if defined (__linux__)
  "/proc/self/exe"
#else // FreeBSD and OSX
  "/proc/curproc/file"
#endif
;

/// brief Returns the filename of the executable belonging to the current process, or 0 if not found.
std::string getexename(char* argv[])
{
    char buf[PATH_MAX];
    
	// Now read the symbolic link
	int ret = readlink(LINK_NAME, buf, PATH_MAX);

	if (ret == -1)
    {
		rMessage() << "getexename: falling back to argv[0]: '" << argv[0] << "'\n";
        
		const char* path = realpath(argv[0], buf);
        
		if (path == nullptr)
        {
			// In case of an error, leave the handling up to the caller
			return std::string();
		}
	}

	/* Ensure proper NUL termination */
	buf[ret] = '\0';

	/* delete the program name */
	*(strrchr(buf, '/')) = '\0';

	// NOTE: we build app path with a trailing '/'
	// it's a general convention in Radiant to have the slash at the end of directories
	if (buf[strlen(buf)-1] != '/')
    {
		strcat(buf, "/");
	}

	return std::string(buf);
}

void ApplicationContextImpl::initialise(int argc, char* argv[]) {
	// Give away unnecessary root privileges.
	// Important: must be done before calling gtk_init().
	char *loginname;
	struct passwd *pw;
	seteuid(getuid());

	if (geteuid() == 0 &&
		(loginname = getlogin()) != 0 &&
		(pw = getpwnam(loginname)) != 0)
	{
		setuid(pw->pw_uid);
	}

	initArgs(argc, argv);

    // Initialise the home directory path
    std::string homedir = getenv("HOME");
    std::string home = os::standardPathWithSlash(homedir) + ".darkradiant/";
    os::makeDirectory(home);
    _homePath = home;

	{
		_appPath = getexename(argv);
        rConsole() << "App Path is " << _appPath << std::endl;
		ASSERT_MESSAGE(!_appPath.empty(), "failed to deduce app path");
	}

	// Initialise the relative paths
	initPaths();
}

// ================ WIN32 ====================
#elif defined(WIN32)

void ApplicationContextImpl::initialise(int argc, char* argv[])
{
	initArgs(argc, argv);
    
    // Get application data directory from environment
	std::string appData = getenv("APPDATA");
	if (appData.empty())
    {
		throw std::runtime_error(
            "Critical: cannot find APPDATA environment variable."
        );
	}

    // Construct DarkRadiant home directory
	_homePath = appData + "\\DarkRadiant";
	if (!os::makeDirectory(_homePath))
    {
        rConsoleError() << "ApplicationContextImpl: could not create home directory "
                  << "'" << _homePath << "'" << std::endl;
    }

	{
		// get path to the editor
		wchar_t filename[MAX_PATH+1];
		GetModuleFileName(0, filename, MAX_PATH);
		wchar_t* last_separator = wcsrchr(filename, '\\');
		if (last_separator != 0) {
			*(last_separator+1) = '\0';
		}
		else {
			filename[0] = '\0';
		}

		// convert to std::string
		std::wstring wide(filename); 
		std::string appPathNarrow(wide.begin(), wide.end());

		// Make sure we have forward slashes
		_appPath = os::standardPath(appPathNarrow);
	}
	// Initialise the relative paths
	initPaths();
}

#else
#error "unsupported platform"
#endif

// ============== OS-Specific Implementations end ===================

void ApplicationContextImpl::initArgs(int argc, char* argv[])
{
	// Store the arguments locally, ignore the first one
	for (int i = 1; i < argc; i++) {
		_cmdLineArgs.push_back(argv[i]);
	}
}

void ApplicationContextImpl::initPaths()
{
	// Ensure that the homepath ends with a slash
	_homePath = os::standardPathWithSlash(_homePath);
	_appPath = os::standardPathWithSlash(_appPath);

	// Make sure the home/settings folder exists (attempt to create it)
	_settingsPath = _homePath;
	if (!os::makeDirectory(_settingsPath))
    {
        rConsoleError() << "ApplicationContextImpl: unable to create settings path '"
                  << _settingsPath << "'" << std::endl;
    }
}

void ApplicationContextImpl::savePathsToRegistry() const {
	GlobalRegistry().set(RKEY_APP_PATH, _appPath);
	GlobalRegistry().set(RKEY_HOME_PATH, _homePath);
	GlobalRegistry().set(RKEY_SETTINGS_PATH, _settingsPath);
	GlobalRegistry().set(RKEY_BITMAPS_PATH, getBitmapsPath());
}

const ErrorHandlingFunction& ApplicationContextImpl::getErrorHandlingFunction() const
{
	return _errorHandler;
}

void ApplicationContextImpl::initErrorHandler()
{
#ifdef _DEBUG
	// Use the PopupErrorHandler, which displays a popup box
	_errorHandler = radiant::PopupErrorHandler::HandleError;

	// Initialise the function pointer in our binary's scope
	GlobalErrorHandler() = _errorHandler;
#endif
}

} // namespace module

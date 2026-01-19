// ExperimentRunner.cpp - Process management implementation
// Copyright (c) 2026 Shane T. Mueller
// Licensed under GPL

#include "ExperimentRunner.h"
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <sys/stat.h>

#ifdef _WIN32
#include <windows.h>
#include <shlobj.h>
#else
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <pwd.h>
#include <fcntl.h>
#endif

ExperimentRunner::ExperimentRunner()
    : mIsRunning(false)
    , mProcessId(0)
    , mLaunchTime(0)
    , mCurrentFullscreen(false)
    , mExitCode(-1)
{
#ifdef _WIN32
    mProcessHandle = nullptr;
    mStdoutPipe = nullptr;
    mStderrPipe = nullptr;
#else
    mStdoutPipe[0] = -1;
    mStdoutPipe[1] = -1;
    mStderrPipe[0] = -1;
    mStderrPipe[1] = -1;
#endif
}

ExperimentRunner::~ExperimentRunner()
{
    if (mIsRunning) {
        Terminate();
    }
}

std::string ExperimentRunner::GetPEBLExecutablePath() const
{
#ifdef _WIN32
    // Windows: Look for pebl2.exe in same directory as launcher or in PATH
    char exePath[MAX_PATH];
    GetModuleFileNameA(NULL, exePath, MAX_PATH);

    // Replace launcher.exe with pebl2.exe
    std::string path(exePath);
    size_t lastSlash = path.find_last_of("\\/");
    if (lastSlash != std::string::npos) {
        path = path.substr(0, lastSlash + 1) + "pebl2.exe";
    } else {
        path = "pebl2.exe";
    }
    return path;
#else
    // Linux/macOS: Try to find pebl2 in same directory as launcher
    char exePath[1024];
    ssize_t len = readlink("/proc/self/exe", exePath, sizeof(exePath) - 1);

    if (len != -1) {
        exePath[len] = '\0';
        std::string path(exePath);
        size_t lastSlash = path.find_last_of('/');
        if (lastSlash != std::string::npos) {
            path = path.substr(0, lastSlash + 1) + "pebl2";
            return path;
        }
    }

    // Fallback: assume pebl2 is in PATH
    return "pebl2";
#endif
}

bool ExperimentRunner::RunExperiment(const std::string& scriptPath,
                                      const std::vector<std::string>& args,
                                      const std::string& subjectCode,
                                      const std::string& language,
                                      bool fullscreen)
{
    if (mIsRunning) {
        printf("Warning: Experiment already running\n");
        return false;
    }

    std::string peblPath = GetPEBLExecutablePath();

    // Extract directory from script path
    std::string workingDir;
    size_t lastSlash = scriptPath.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        workingDir = scriptPath.substr(0, lastSlash);
    } else {
        workingDir = ".";
    }

    // Get just the filename for passing to PEBL
    std::string scriptFilename;
    if (lastSlash != std::string::npos) {
        scriptFilename = scriptPath.substr(lastSlash + 1);
    } else {
        scriptFilename = scriptPath;
    }

    // Build complete argument list with PEBL command-line flags
    std::vector<std::string> fullArgs;

    // Add user-provided args first (these may include -v for positional params)
    for (const auto& arg : args) {
        fullArgs.push_back(arg);
    }

    // Add subject code with -s flag
    if (!subjectCode.empty()) {
        fullArgs.push_back("-s");
        fullArgs.push_back(subjectCode);
    }

    // Add language if specified
    if (!language.empty()) {
        fullArgs.push_back("--language");
        fullArgs.push_back(language);
    }

    // Add fullscreen flag
    if (fullscreen) {
        fullArgs.push_back("--fullscreen");
    }

#ifdef _WIN32
    // Windows: Use CreateProcess
    std::string cmdLine = "\"" + peblPath + "\" \"" + scriptFilename + "\"";

    // Add all arguments
    for (const auto& arg : fullArgs) {
        cmdLine += " ";
        if (arg.find(' ') != std::string::npos) {
            cmdLine += "\"" + arg + "\"";
        } else {
            cmdLine += arg;
        }
    }

    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // Create the child process with working directory set
    if (!CreateProcessA(
            NULL,                       // Application name
            const_cast<char*>(cmdLine.c_str()),  // Command line
            NULL,                       // Process security attributes
            NULL,                       // Thread security attributes
            FALSE,                      // Inherit handles
            0,                          // Creation flags
            NULL,                       // Environment
            workingDir.c_str(),        // Current directory (IMPORTANT!)
            &si,                        // Startup info
            &pi))                       // Process info
    {
        printf("Failed to create process: %s\n", cmdLine.c_str());
        return false;
    }

    mProcessHandle = pi.hProcess;
    mProcessId = pi.dwProcessId;
    CloseHandle(pi.hThread);
    mIsRunning = true;

#else
    // Unix: Create pipes for stdout/stderr capture
    if (pipe(mStdoutPipe) < 0 || pipe(mStderrPipe) < 0) {
        printf("Failed to create pipes\n");
        return false;
    }

    // Make read ends non-blocking
    fcntl(mStdoutPipe[0], F_SETFL, O_NONBLOCK);
    fcntl(mStderrPipe[0], F_SETFL, O_NONBLOCK);

    // Clear output buffers
    mStdoutBuffer.clear();
    mStderrBuffer.clear();

    pid_t pid = fork();

    if (pid < 0) {
        printf("Failed to fork process\n");
        close(mStdoutPipe[0]);
        close(mStdoutPipe[1]);
        close(mStderrPipe[0]);
        close(mStderrPipe[1]);
        return false;
    }

    if (pid == 0) {
        // Child process - redirect stdout/stderr to pipes
        close(mStdoutPipe[0]);  // Close read ends
        close(mStderrPipe[0]);

        dup2(mStdoutPipe[1], STDOUT_FILENO);
        dup2(mStderrPipe[1], STDERR_FILENO);

        close(mStdoutPipe[1]);
        close(mStderrPipe[1]);

        // Change to working directory
        if (chdir(workingDir.c_str()) != 0) {
            fprintf(stderr, "Failed to change directory to: %s\n", workingDir.c_str());
            exit(1);
        }

        std::vector<char*> argv;
        argv.push_back(const_cast<char*>(peblPath.c_str()));
        argv.push_back(const_cast<char*>(scriptFilename.c_str()));

        for (const auto& arg : fullArgs) {
            argv.push_back(const_cast<char*>(arg.c_str()));
        }

        argv.push_back(nullptr);

        execvp(peblPath.c_str(), argv.data());

        // If we get here, exec failed
        fprintf(stderr, "Failed to execute: %s\n", peblPath.c_str());
        exit(1);
    }

    // Parent process - close write ends
    close(mStdoutPipe[1]);
    close(mStderrPipe[1]);
    mStdoutPipe[1] = -1;
    mStderrPipe[1] = -1;

    mProcessId = pid;
    mIsRunning = true;
#endif

    printf("Launched experiment: %s (PID: %d) in directory: %s\n",
           scriptFilename.c_str(), mProcessId, workingDir.c_str());

    // Log the launch
    mCurrentScript = scriptPath;
    mCurrentSubject = subjectCode;
    mCurrentLanguage = language;
    mCurrentFullscreen = fullscreen;
    mLaunchTime = std::time(nullptr);
    LogLaunch(scriptPath, subjectCode, language, fullscreen);

    return true;
}

int ExperimentRunner::WaitForCompletion()
{
    if (!mIsRunning) {
        return -1;
    }

#ifdef _WIN32
    WaitForSingleObject(mProcessHandle, INFINITE);

    DWORD exitCode;
    GetExitCodeProcess(mProcessHandle, &exitCode);
    mExitCode = static_cast<int>(exitCode);
    CloseHandle(mProcessHandle);

    mProcessHandle = nullptr;
    mIsRunning = false;

    LogCompletion(mExitCode);
    return mExitCode;
#else
    int status;
    waitpid(mProcessId, &status, 0);

    // Do final read of any remaining output
    UpdateOutput();

    // Close pipes
    if (mStdoutPipe[0] >= 0) {
        close(mStdoutPipe[0]);
        mStdoutPipe[0] = -1;
    }
    if (mStderrPipe[0] >= 0) {
        close(mStderrPipe[0]);
        mStderrPipe[0] = -1;
    }

    mIsRunning = false;

    mExitCode = -1;
    if (WIFEXITED(status)) {
        mExitCode = WEXITSTATUS(status);
    }

    LogCompletion(mExitCode);
    return mExitCode;
#endif
}

void ExperimentRunner::Terminate()
{
    if (!mIsRunning) {
        return;
    }

#ifdef _WIN32
    TerminateProcess(mProcessHandle, 1);
    CloseHandle(mProcessHandle);
    mProcessHandle = nullptr;
#else
    kill(mProcessId, SIGTERM);

    // Close pipes
    if (mStdoutPipe[0] >= 0) {
        close(mStdoutPipe[0]);
        mStdoutPipe[0] = -1;
    }
    if (mStderrPipe[0] >= 0) {
        close(mStderrPipe[0]);
        mStderrPipe[0] = -1;
    }
#endif

    mIsRunning = false;
    LogCompletion(-999);  // Log termination with special exit code
    printf("Terminated experiment (PID: %d)\n", mProcessId);
}

std::string ExperimentRunner::GetLaunchLogPath()
{
    std::string logPath;

#ifdef _WIN32
    // Windows: Documents folder
    char docPath[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, 0, docPath))) {
        logPath = std::string(docPath);
    }
#else
    // Linux/macOS: ~/Documents or ~ if Documents doesn't exist
    const char* homeDir = getenv("HOME");
    if (!homeDir) {
        struct passwd* pw = getpwuid(getuid());
        homeDir = pw->pw_dir;
    }

    std::string docDir = std::string(homeDir) + "/Documents";
    struct stat st;
    if (stat(docDir.c_str(), &st) == 0 && S_ISDIR(st.st_mode)) {
        logPath = docDir;
    } else {
        logPath = homeDir;
    }
#endif

    // Look for pebl-exp.X.X directories (newest first)
    const char* versions[] = {
        "pebl-exp.2.3",
        "pebl-exp.2.2",
        "pebl-exp.2.1",
        "pebl-exp.2.0",
        "pebl-exp.0.14",
        "pebl-exp"
    };

#ifdef _WIN32
    const char* separator = "\\";
#else
    const char* separator = "/";
#endif

    for (const char* version : versions) {
        std::string peblPath = logPath + separator + version;
        struct stat st;
        if (stat(peblPath.c_str(), &st) == 0 && S_ISDIR(st.st_mode)) {
            // Check if logs subdirectory exists, create if needed
            std::string logsPath = peblPath + separator + "logs";
            struct stat logsSt;
            if (stat(logsPath.c_str(), &logsSt) != 0 || !S_ISDIR(logsSt.st_mode)) {
                // Create logs directory
#ifdef _WIN32
                CreateDirectoryA(logsPath.c_str(), NULL);
#else
                mkdir(logsPath.c_str(), 0755);
#endif
            }
            return logsPath + separator + "launcher-log.csv";
        }
    }

    // Fallback to home directory
    return logPath + separator + "launcher-log.csv";
}

void ExperimentRunner::LogLaunch(const std::string& scriptPath,
                                  const std::string& subjectCode,
                                  const std::string& language,
                                  bool fullscreen)
{
    std::string logPath = GetLaunchLogPath();
    bool fileExists = false;

    // Check if file already exists
    struct stat st;
    if (stat(logPath.c_str(), &st) == 0) {
        fileExists = true;
    }

    std::ofstream logFile(logPath, std::ios::app);
    if (!logFile.is_open()) {
        printf("Warning: Could not open launch log file: %s\n", logPath.c_str());
        return;
    }

    // Write header if new file
    if (!fileExists) {
        logFile << "timestamp,experiment_path,subject_code,language,fullscreen,exit_status,duration_seconds" << std::endl;
    }

    // Write launch record (completion will add exit_status and duration)
    char timeBuf[64];
    struct tm* timeinfo = std::localtime(&mLaunchTime);
    std::strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M:%S", timeinfo);

    logFile << timeBuf << ","
            << "\"" << scriptPath << "\","
            << "\"" << subjectCode << "\","
            << "\"" << language << "\","
            << (fullscreen ? "true" : "false") << ","
            << "RUNNING,"
            << "0" << std::endl;

    logFile.close();
}

void ExperimentRunner::LogCompletion(int exitStatus)
{
    std::string logPath = GetLaunchLogPath();

    // Calculate duration
    std::time_t endTime = std::time(nullptr);
    int duration = static_cast<int>(endTime - mLaunchTime);

    // Read the entire log file
    std::ifstream inFile(logPath);
    if (!inFile.is_open()) {
        return;
    }

    std::stringstream buffer;
    std::string line;
    std::string lastLine;
    bool foundRunning = false;

    // Read all lines and update the last RUNNING entry
    while (std::getline(inFile, line)) {
        if (line.find("RUNNING") != std::string::npos && !foundRunning) {
            // This is the running entry we want to update
            foundRunning = true;

            // Replace RUNNING with exit status and 0 with duration
            size_t pos = line.find("RUNNING");
            if (pos != std::string::npos) {
                line.replace(pos, 7, std::to_string(exitStatus));
            }

            // Replace the last 0 (duration) with actual duration
            pos = line.rfind(",0");
            if (pos != std::string::npos) {
                line = line.substr(0, pos + 1) + std::to_string(duration);
            }
        }
        buffer << line << std::endl;
    }
    inFile.close();

    // Write back the updated log
    std::ofstream outFile(logPath);
    if (outFile.is_open()) {
        outFile << buffer.str();
        outFile.close();
    }
}

void ExperimentRunner::UpdateOutput()
{
#ifdef _WIN32
    // TODO: Windows pipe reading
#else
    // Read from stdout pipe (non-blocking)
    if (mStdoutPipe[0] >= 0) {
        char buffer[4096];
        ssize_t bytesRead;
        while ((bytesRead = read(mStdoutPipe[0], buffer, sizeof(buffer) - 1)) > 0) {
            buffer[bytesRead] = '\0';
            mStdoutBuffer.append(buffer);
        }
    }

    // Read from stderr pipe (non-blocking)
    if (mStderrPipe[0] >= 0) {
        char buffer[4096];
        ssize_t bytesRead;
        while ((bytesRead = read(mStderrPipe[0], buffer, sizeof(buffer) - 1)) > 0) {
            buffer[bytesRead] = '\0';
            mStderrBuffer.append(buffer);
        }
    }
#endif
}

bool ExperimentRunner::IsRunning()
{
    if (!mIsRunning) {
        return false;
    }

#ifdef _WIN32
    // Windows: Check if process handle is still valid and running
    if (!mProcessHandle) {
        mIsRunning = false;
        return false;
    }

    DWORD exitCode;
    if (GetExitCodeProcess(mProcessHandle, &exitCode)) {
        if (exitCode == STILL_ACTIVE) {
            return true;
        } else {
            // Process has finished
            mExitCode = static_cast<int>(exitCode);
            printf("Process %lu finished with exit code %d\n", mProcessId, mExitCode);
            CloseHandle(mProcessHandle);
            mProcessHandle = nullptr;
            mIsRunning = false;
            LogCompletion(mExitCode);
            return false;
        }
    } else {
        // Error querying process - assume it's dead
        CloseHandle(mProcessHandle);
        mProcessHandle = nullptr;
        mIsRunning = false;
        return false;
    }
#else
    // Unix: Use waitpid with WNOHANG to check without blocking
    // This is exactly what PEBL's CheckProcessStatus() does
    int status;
    pid_t result = waitpid(mProcessId, &status, WNOHANG);

    if (result == 0) {
        // Process still running
        return true;
    }
    else if (result == mProcessId) {
        // Process has finished
        printf("Process %d finished\n", mProcessId);

        // Do final read of any remaining output
        UpdateOutput();

        // Close pipes
        if (mStdoutPipe[0] >= 0) {
            close(mStdoutPipe[0]);
            mStdoutPipe[0] = -1;
        }
        if (mStderrPipe[0] >= 0) {
            close(mStderrPipe[0]);
            mStderrPipe[0] = -1;
        }

        mIsRunning = false;

        mExitCode = -1;
        if (WIFEXITED(status)) {
            mExitCode = WEXITSTATUS(status);
            printf("  Exit code: %d\n", mExitCode);
        } else if (WIFSIGNALED(status)) {
            printf("  Terminated by signal %d\n", WTERMSIG(status));
        }

        LogCompletion(mExitCode);
        return false;
    }
    else {
        // Error or already reaped (result == -1)
        printf("waitpid error for PID %d (already reaped or error)\n", mProcessId);
        mIsRunning = false;
        return false;
    }
#endif
}

// ExperimentRunner.h - Process management for running PEBL experiments
// Copyright (c) 2026 Shane T. Mueller
// Licensed under GPL

#ifndef EXPERIMENT_RUNNER_H
#define EXPERIMENT_RUNNER_H

#include <string>
#include <vector>
#include <ctime>

class LauncherConfig;

class ExperimentRunner {
public:
    ExperimentRunner();
    ExperimentRunner(LauncherConfig* config);
    ~ExperimentRunner();

    // Run an experiment with given arguments
    // Returns true if process was launched successfully
    bool RunExperiment(const std::string& scriptPath,
                       const std::vector<std::string>& args,
                       const std::string& subjectCode = "",
                       const std::string& language = "",
                       bool fullscreen = false);

    // Check if an experiment is currently running
    // This method actually polls the process status non-blocking
    bool IsRunning();

    // Wait for running experiment to complete
    int WaitForCompletion();

    // Terminate running experiment
    void Terminate();

    // Get launch log file path
    static std::string GetLaunchLogPath();

    // Get captured stdout/stderr output
    std::string GetStdout() const { return mStdoutBuffer; }
    std::string GetStderr() const { return mStderrBuffer; }

    // Get exit code from last completed process
    // Returns -1 if process is still running or hasn't been run
    int GetExitCode() const { return mExitCode; }

    // Read available output from running process (non-blocking)
    void UpdateOutput();

private:
    std::string GetPEBLExecutablePath() const;
    void LogLaunch(const std::string& scriptPath, const std::string& subjectCode,
                   const std::string& language, bool fullscreen);
    void LogCompletion(int exitStatus);

    LauncherConfig* mConfig;  // Optional config for executable path
    bool mIsRunning;
    std::time_t mLaunchTime;
    std::string mCurrentScript;
    std::string mCurrentSubject;
    std::string mCurrentLanguage;
    bool mCurrentFullscreen;
    int mExitCode;  // Exit code from last completed process (-1 if not finished)

    // Output capture
    std::string mStdoutBuffer;
    std::string mStderrBuffer;

#ifdef _WIN32
    void* mProcessHandle;  // HANDLE on Windows
    unsigned long mProcessId;  // DWORD on Windows
    void* mStdoutReadPipe;   // Read end of stdout pipe (parent reads from this)
    void* mStdoutWritePipe;  // Write end of stdout pipe (child writes to this)
    void* mStderrReadPipe;   // Read end of stderr pipe (parent reads from this)
    void* mStderrWritePipe;  // Write end of stderr pipe (child writes to this)
#else
    int mProcessId;  // pid_t on Unix
    int mStdoutPipe[2];  // Pipe for stdout [read, write]
    int mStderrPipe[2];  // Pipe for stderr [read, write]
#endif
};

#endif // EXPERIMENT_RUNNER_H

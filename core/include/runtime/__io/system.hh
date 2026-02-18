///--- The Kairo Project ------------------------------------------------------------------------///
///                                                                                              ///
///   Part of the Kairo Project, under the Attribution 4.0 International license (CC BY 4.0).    ///
///   You are allowed to use, modify, redistribute, and create derivative works, even for        ///
///   commercial purposes, provided that you give appropriate credit, and indicate if changes    ///
///   were made.                                                                                 ///
///                                                                                              ///
///   For more information on the license terms and requirements, please visit:                  ///
///     https://creativecommons.org/licenses/by/4.0/                                             ///
///                                                                                              ///
///   SPDX-License-Identifier: CC-BY-4.0                                                         ///
///   Copyright (c) 2024 The Kairo Project (CC BY 4.0)                                           ///
///                                                                                              ///
///-------------------------------------------------------------------------------- lib-helix ---///
///
/// Could I have made this a lot better by spliting unix and windows into 2 source files?
/// yes
///
/// Should I have done that?
/// yes.
///
/// Will I do that?
/// no
///
///-------------------------------------------------------------------------------- lib-helix ---///

#ifndef _$_HX_CORE_M6SYSTEM
#define _$_HX_CORE_M6SYSTEM

#include <include/config/config.hh>
#include <include/runtime/__error/runtime_error.hh>
#include <include/runtime/__panic/panic_config.hh>
#include <include/types/question/question_impl.hh>
#include <include/types/string/string.hh>

#ifdef __linux__
#   include <sys/wait.h>
#   include <unistd.h>
#endif

// "⠈", "⠐", "⠠", "⢀", "⡀", "⠄", "⠂", "⠁"

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

struct ProcessOutput {
    string stdout_data;
    string stderr_data;

    i32   return_code{};
    isize pid{};

    libcxx::chrono::steady_clock::time_point start_time;
    libcxx::chrono::steady_clock::time_point end_time;

    $function<void(string::slice)> on_stdout;
    $function<void(string::slice)> on_stderr;

    bool timed_out{false};
    bool terminated{false};

    ProcessOutput() = default;

    ProcessOutput(const ProcessOutput &other) noexcept
        : stdout_data(other.stdout_data)
        , stderr_data(other.stderr_data)
        , return_code(other.return_code)
        , pid(other.pid)
        , start_time(other.start_time)
        , end_time(other.end_time)
        , on_stdout(other.on_stdout)
        , on_stderr(other.on_stderr)
        , timed_out(other.timed_out)
        , terminated(other.terminated) {}

    ProcessOutput(ProcessOutput &&other) noexcept
        : stdout_data(std::Memory::move(other.stdout_data))
        , stderr_data(std::Memory::move(other.stderr_data))
        , return_code(other.return_code)
        , pid(other.pid)
        , start_time(other.start_time)
        , end_time(other.end_time)
        , on_stdout(std::Memory::move(other.on_stdout))
        , on_stderr(std::Memory::move(other.on_stderr))
        , timed_out(other.timed_out)
        , terminated(other.terminated) {}

    ProcessOutput &operator=(const ProcessOutput &other) noexcept {
        stdout_data = other.stdout_data;
        stderr_data = other.stderr_data;
        return_code = other.return_code;
        pid         = other.pid;
        start_time  = other.start_time;
        end_time    = other.end_time;
        on_stdout   = other.on_stdout;
        on_stderr   = other.on_stderr;
        timed_out   = other.timed_out;
        terminated  = other.terminated;
        return *this;
    }

    ProcessOutput &operator=(ProcessOutput &&other) noexcept {
        stdout_data = std::Memory::move(other.stdout_data);
        stderr_data = std::Memory::move(other.stderr_data);
        return_code = other.return_code;
        pid         = other.pid;
        start_time  = other.start_time;
        end_time    = other.end_time;
        on_stdout   = std::Memory::move(other.on_stdout);
        on_stderr   = std::Memory::move(other.on_stderr);
        timed_out   = other.timed_out;
        terminated  = other.terminated;
        return *this;
    }

    ~ProcessOutput() = default;

    ProcessOutput &on_fail($function<void()> cb, bool include_timeout = false) {
        on_fail_ = {include_timeout, cb};

        if (((terminated && return_code != 0) || (timed_out && include_timeout))) {
            libcxx::get<1>(on_fail_)();
        }

        return *this;
    }

    ProcessOutput &on_timeout($function<void()> cb) {
        on_timeout_ = cb;

        if (timed_out) {
            on_timeout_();
        }

        return *this;
    }

    ProcessOutput &on_success($function<void()> cb) {
        on_success_ = cb;

        if (terminated && return_code == 0) {
            on_success_();
        }

        return *this;
    }

  private:
    libcxx::mutex stdout_mutex;
    libcxx::mutex stderr_mutex;

    libcxx::pair<bool, $function<void()>> on_fail_;
    $function<void()>                     on_timeout_;
    $function<void()>                     on_success_;

    void call_backs() {
        if (((terminated && return_code != 0) || (timed_out && libcxx::get<0>(on_fail_)))) {
            libcxx::get<1>(on_fail_)();
        }

        if (timed_out) {
            on_timeout_();
        }

        if (terminated && return_code == 0) {
            on_success_();
        }
    }

    void terminate(bool st) { this->terminated = st; }

    void timeout(bool st) { this->timed_out = st; }

    void append_stdout(const char *data, size_t n) {
        libcxx::lock_guard<libcxx::mutex> lg(stdout_mutex);
        auto                              str = cstring_to_string(libcxx::string(data, n));
        stdout_data.append(str);
        if (on_stdout) {
            on_stdout(stdout_data.subslice(stdout_data.size() - n, n));
        }
    }

    void append_stderr(const char *data, size_t n) {
        libcxx::lock_guard<libcxx::mutex> lg(stderr_mutex);
        auto                              str = cstring_to_string(libcxx::string(data, n));
        stderr_data.append(str);
        if (on_stderr) {
            on_stderr(stderr_data.subslice(stderr_data.size() - n, n));
        }
    }

    friend class Subprocess;
};

class Subprocess {
  public:
    // non-copyable
    Subprocess(const Subprocess &)            = delete;
    Subprocess &operator=(const Subprocess &) = delete;

    // movable
    Subprocess(Subprocess &&other) noexcept { move_from(std::Memory::move(other)); }
    Subprocess &operator=(Subprocess &&other) noexcept {
        if (this != &other) {
            cleanup();
            move_from(std::Memory::move(other));
        }
        return *this;
    }

    ~Subprocess() {
        try {
            cleanup();
        } catch (...) {
            // no-throw dtor
        }
    }

    // launches a process with the provided command (wide-string logical command in your API).
    // non-blocking: immediately returns. Output is streamed via internal threads.
    // you can poll output via output(), or set callbacks in output() before launching for streaming
    // consumption.
    static Subprocess run(const string             &wcmd,
                          bool                      capture_output,
                          const map<string, string> env         = {},
                          const string             *working_dir = nullptr) {
        Subprocess p;
        p.start_time_ = libcxx::chrono::steady_clock::now();
        p.launch(wcmd, env, working_dir, capture_output);
        return p;
    }

    // wait until the process exits or timeout_ms elapses.
    // timeout_ms <= 0 means wait indefinitely.
    // returns true if process exited, false if timeout occurred.
    bool wait(int timeout_ms = -1) {
        if (!running_) {
            return true;
        }

#if defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64)
        DWORD dwTimeout = (timeout_ms < 0) ? INFINITE : static_cast<DWORD>(timeout_ms);
        DWORD wr        = WaitForSingleObject(pi_.hProcess, dwTimeout);
        if (wr == WAIT_TIMEOUT) {
            return false;
        }
        if (wr != WAIT_OBJECT_0) {
            throw libcxx::runtime_error("WaitForSingleObject failed! Error: " +
                                        libcxx::to_string(GetLastError()));
        }
        finalize_windows(true);
        return true;
#else
        // poll with waitpid(WNOHANG) loop if timeout specified.
        if (timeout_ms < 0) {
            blocking_waitpid();
            return true;
        }
        auto deadline =
            libcxx::chrono::steady_clock::now() + libcxx::chrono::milliseconds(timeout_ms);
        while (libcxx::chrono::steady_clock::now() < deadline) {
            int   status = 0;
            pid_t r      = ::waitpid(pid_, &status, WNOHANG);
            if (r == -1) {
                int e = errno;
                throw libcxx::runtime_error("waitpid failed: errno=" + libcxx::to_string(e));
            }
            if (r == pid_) {
                finalize_posix(status);
                return true;
            }
            libcxx::this_thread::sleep_for(libcxx::chrono::milliseconds(5));
        }
        return false;
#endif
    }

    // terminate the process (best-effort). Safe to call multiple times.
    void terminate(int exit_code = 1) {
        if (!running_) {
            return;
        }

#if defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64)
        TerminateProcess(pi_.hProcess, static_cast<UINT>(exit_code));
        // we still need to join readers and collect exit status.
        finalize_windows(/*mark_terminated=*/true);
#else
        ::kill(pid_, SIGTERM);
        // if it doesn't exit quickly, SIGKILL as last resort.
        for (int i = 0; i < 50 && running_; ++i) {
            libcxx::this_thread::sleep_for(libcxx::chrono::milliseconds(10));
        }
        if (running_) {
            ::kill(pid_, SIGKILL);
        }
        // ensure we reap it.
        int status = 0;
        (void)::waitpid(pid_, &status, 0);
        finalize_posix(status, /*mark_terminated=*/true);
#endif
    }

    // convenience: wait with timeout; if it times out, terminate and return unified output.
    ProcessOutput timeout(int timeout_ms) {
        ProcessOutput pr{};
        bool          ok = wait(timeout_ms);
        if (!ok) {
            timed_out_ = true;
            terminate(1);
        }
        pr = output();
        return pr;
    }

    // collect output after process has finished; returns the unified ProcessOutput
    ProcessOutput output() {
        if (running_) {
            wait();
        }
        ProcessOutput res;
        {
            libcxx::lock_guard<libcxx::mutex> lg_out(output_.stdout_mutex);
            res.stdout_data = output_.stdout_data;
        }
        {
            libcxx::lock_guard<libcxx::mutex> lg_err(output_.stderr_mutex);
            res.stderr_data = output_.stderr_data;
        }
        res.return_code = return_code_;
        res.pid         = pid_public_;
        res.start_time  = start_time_;
        res.end_time    = end_time_;
        res.timeout(timed_out_);
        res.terminate(was_terminated_);
        return res;
    }

    // accessors

    // non-blocking: you can read accumulated stdout/stderr while process runs.
    const ProcessOutput &output() const { return output_; }

    // register or replace streaming callbacks at any time.
    void stdout_callback($function<void(string::slice)> cb) {
        libcxx::lock_guard<libcxx::mutex> lg(output_.stdout_mutex);
        output_.on_stdout = std::Memory::move(cb);
    }
    void stderr_callback($function<void(string::slice)> cb) {
        libcxx::lock_guard<libcxx::mutex> lg(output_.stderr_mutex);
        output_.on_stderr = std::Memory::move(cb);
    }
    // Back-compat: keep old names
    void set_stdout_callback($function<void(string::slice)> cb) {
        stdout_callback(std::Memory::move(cb));
    }
    void set_stderr_callback($function<void(string::slice)> cb) {
        stderr_callback(std::Memory::move(cb));
    }

    bool      is_running() const { return running_; }
    long long pid() const { return static_cast<long long>(pid_public_); }

  private:
    Subprocess() = default;

    void move_from(Subprocess &&o) {
        running_.store(o.running_.load());
        return_code_    = o.return_code_;
        timed_out_      = o.timed_out_;
        was_terminated_ = o.was_terminated_;
        start_time_     = o.start_time_;
        end_time_       = o.end_time_;
        pid_public_     = o.pid_public_;
        output_         = std::Memory::move(o.output_);

#if defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64)
        pi_          = o.pi_;
        o.pi_        = PROCESS_INFORMATION{};
        hStdoutRd_   = o.hStdoutRd_;
        o.hStdoutRd_ = nullptr;
        hStdoutWr_   = o.hStdoutWr_;
        o.hStdoutWr_ = nullptr;
        hStderrRd_   = o.hStderrRd_;
        o.hStderrRd_ = nullptr;
        hStderrWr_   = o.hStderrWr_;
        o.hStderrWr_ = nullptr;
#else
        pid_      = o.pid_;
        o.pid_    = -1;
        out_rd_   = o.out_rd_;
        o.out_rd_ = -1;
        err_rd_   = o.err_rd_;
        o.err_rd_ = -1;
        out_wr_   = o.out_wr_;
        o.out_wr_ = -1;
        err_wr_   = o.err_wr_;
        o.err_wr_ = -1;
#endif

        t_out_ = std::Memory::move(o.t_out_);
        t_err_ = std::Memory::move(o.t_err_);
        o.running_.store(false);
    }

    void cleanup() {
        // ensure threads joined and handles closed.
        if (running_) {
            // best-effort soft terminate to avoid leaks.
            try {
                terminate(1);
            } catch (...) {}  // NOLINT(bugprone-empty-catch)
        }
        join_readers();
#if defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64)
        if (hStdoutRd_)
            CloseHandle(hStdoutRd_), hStdoutRd_ = nullptr;
        if (hStdoutWr_)
            CloseHandle(hStdoutWr_), hStdoutWr_ = nullptr;
        if (hStderrRd_)
            CloseHandle(hStderrRd_), hStderrRd_ = nullptr;
        if (hStderrWr_)
            CloseHandle(hStderrWr_), hStderrWr_ = nullptr;
        if (pi_.hProcess)
            CloseHandle(pi_.hProcess), pi_.hProcess = nullptr;
        if (pi_.hThread)
            CloseHandle(pi_.hThread), pi_.hThread = nullptr;
#else
        if (out_rd_ >= 0) {
            ::close(out_rd_), out_rd_ = -1;
        }
        if (err_rd_ >= 0) {
            ::close(err_rd_), err_rd_ = -1;
        }
        if (out_wr_ >= 0) {
            ::close(out_wr_), out_wr_ = -1;
        }
        if (err_wr_ >= 0) {
            ::close(err_wr_), err_wr_ = -1;
        }
#endif
    }

    void join_readers() {
        if (t_out_.joinable()) {
            t_out_.join();
        }
        if (t_err_.joinable()) {
            t_err_.join();
        }
    }

    void mark_finished() {
        running_  = false;
        end_time_ = libcxx::chrono::steady_clock::now();
        output_.terminate(was_terminated_);
    }

    void launch(const string              &wcmd,
                const map<string, string> &m_env,
                const string              *working_dir,
                bool                       capture_output) {
        vec<string> env;

        if (!m_env.empty()) {
            env.reserve(m_env.size());
            for (const auto &kv : m_env) {
                env.push_back(kv.first + L"=" + kv.second);
            }
        }

#if defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64)
        SECURITY_ATTRIBUTES sa{};
        sa.nLength        = sizeof(SECURITY_ATTRIBUTES);
        sa.bInheritHandle = TRUE;

        if (!CreatePipe(&hStdoutRd_, &hStdoutWr_, &sa, 0))
            throw libcxx::runtime_error("CreatePipe stdout failed! Error: " +
                                        libcxx::to_string(GetLastError()));
        if (!SetHandleInformation(hStdoutRd_, HANDLE_FLAG_INHERIT, 0)) {
            CloseHandle(hStdoutRd_);
            CloseHandle(hStdoutWr_);
            throw libcxx::runtime_error("SetHandleInformation stdout failed! Error: " +
                                        libcxx::to_string(GetLastError()));
        }

        if (!CreatePipe(&hStderrRd_, &hStderrWr_, &sa, 0)) {
            CloseHandle(hStdoutRd_);
            CloseHandle(hStdoutWr_);
            throw libcxx::runtime_error("CreatePipe stderr failed! Error: " +
                                        libcxx::to_string(GetLastError()));
        }
        if (!SetHandleInformation(hStderrRd_, HANDLE_FLAG_INHERIT, 0)) {
            CloseHandle(hStdoutRd_);
            CloseHandle(hStdoutWr_);
            CloseHandle(hStderrRd_);
            CloseHandle(hStderrWr_);
            throw libcxx::runtime_error("SetHandleInformation stderr failed! Error: " +
                                        libcxx::to_string(GetLastError()));
        }

        STARTUPINFOW si{};
        si.cb         = sizeof(si);
        si.hStdOutput = hStdoutWr_;
        si.hStdError  = hStderrWr_;
        si.dwFlags |= STARTF_USESTDHANDLES;

        // note: If you need stdin, add a pipe and set si.hStdInput.
        PROCESS_INFORMATION pi{};
        // convert command to wide.
        const libcxx::wstring& wcmd_w(
            wcmd.raw_string());  // wcmd is "libcxx::string" but contains wide-logic; your helpers
                                 // are named cstring_to_string/string_to_cstring; here we assume
                                 // wcmd already logical wide.
        // if wcmd actually contains wide characters stored in narrow string, replace with your
        // cstring_to_string appropriately.

        // working directory
        libcxx::wstring wdir_w;
        LPCWSTR         lpCurrentDirectory = nullptr;
        if ((working_dir != nullptr) && !working_dir->empty()) {
            wdir_w.assign(working_dir->begin(), working_dir->end());
            lpCurrentDirectory = wdir_w.c_str();
        }

        // environment block (optional). If env empty, inherit.
        // windows expects a double-null-terminated block of WCHAR pairs "Var=Value\0... \0\0"
        libcxx::wstring                 env_block;
        LPWSTR                          lpEnvironment = nullptr;
        libcxx::vector<libcxx::wstring> env_entries;

        if (!env.empty()) {
            for (auto &e : env) {
                // expect "KEY=VALUE" in UTF-8ish container; convert.
                libcxx::wstring we(e.raw_string());
                env_entries.push_back(std::Memory::move(we));
            }

            size_t total = 0;
            for (auto &s : env_entries) {
                total += (s.size() + 1);
            }

            env_block.reserve(total + 1);

            for (auto &s : env_entries) {
                env_block.append(s);
                env_block.push_back(L'\0');
            }

            env_block.push_back(L'\0');
            lpEnvironment = const_cast<wchar_t *>(env_block.c_str());
        }

        // CreateProcessW requires writable command buffer
        libcxx::wstring cmdline = wcmd_w;
        if (!CreateProcessW(
                /*lpApplicationName*/ nullptr,
                /*lpCommandLine*/ cmdline.empty() ? nullptr : cmdline.data(),
                /*lpProcessAttributes*/ nullptr,
                /*lpThreadAttributes*/ nullptr,
                /*bInheritHandles*/ TRUE,
                /*dwCreationFlags*/ CREATE_NO_WINDOW |
                    ((lpEnvironment != nullptr) ? CREATE_UNICODE_ENVIRONMENT : 0),
                /*lpEnvironment*/ lpEnvironment,
                /*lpCurrentDirectory*/ lpCurrentDirectory,
                /*lpStartupInfo*/ &si,
                /*lpProcessInformation*/ &pi)) {

            DWORD err = GetLastError();

            CloseHandle(hStdoutRd_);
            CloseHandle(hStdoutWr_);
            CloseHandle(hStderrRd_);
            CloseHandle(hStderrWr_);

            throw libcxx::runtime_error("CreateProcessW failed! Error: " + libcxx::to_string(err));
        }

        // child holds these write ends; parent will close them after spawn
        CloseHandle(hStdoutWr_);
        hStdoutWr_ = nullptr;
        CloseHandle(hStderrWr_);
        hStderrWr_ = nullptr;

        pi_         = pi;
        running_    = true;
        pid_public_ = static_cast<long long>(GetProcessId(pi.hProcess));

        // reader threads
        t_out_ = libcxx::thread([capture_output, this]() {
            read_pipe_windows(hStdoutRd_, /*is_stdout=*/true, capture_output);
        });
        t_err_ = libcxx::thread([capture_output, this]() {
            read_pipe_windows(hStderrRd_, /*is_stdout=*/false, capture_output);
        });

#else
        int out_pipe[2]{-1, -1};  // NOLINT
        int err_pipe[2]{-1, -1};  // NOLINT

        if (pipe(static_cast<int *>(out_pipe)) < 0) {
            throw libcxx::runtime_error("pipe() stdout failed");
        }

        if (pipe(static_cast<int *>(err_pipe)) < 0) {
            ::close(out_pipe[0]);
            ::close(out_pipe[1]);
            throw libcxx::runtime_error("pipe() stderr failed");
        }

        pid_t pid = fork();

        if (pid < 0) {
            ::close(out_pipe[0]);
            ::close(out_pipe[1]);
            ::close(err_pipe[0]);
            ::close(err_pipe[1]);

            throw libcxx::runtime_error("fork() failed");
        }

        if (pid == 0) {
            // child
            // redirect stdout/stderr
            ::close(out_pipe[0]);  // close read
            ::close(err_pipe[0]);
            ::dup2(out_pipe[1], STDOUT_FILENO);
            ::dup2(err_pipe[1], STDERR_FILENO);
            ::close(out_pipe[1]);
            ::close(err_pipe[1]);

            // optional working dir
            if ((working_dir != nullptr) && !working_dir->empty()) {
                if (chdir(string_to_cstring(*working_dir).c_str()) != 0) {
                    _exit(127);
                }
            }

            // build command for /bin/sh -c to keep parity with shell command interpretation,
            // or exec directly if you want no shell. Here we mimic your original system() usage.
            libcxx::string cmd = string_to_cstring(wcmd);

            // environment setup
            if (!env.empty()) {
                // construct new environment
                // note: setenv() is easier than building envp; do before exec.
                for (const auto &e : env) {
                    auto pos = e.lfind(L"=");
                    if (pos != libcxx::string::npos) {
                        auto key = string_to_cstring(e.raw_string().substr(0, pos));
                        auto val = string_to_cstring(e.raw_string().substr(pos + 1));
                        setenv(key.c_str(), val.c_str(), 1);
                    }
                }
            }

            // use sh -c "cmd"
            execl("/bin/sh", "sh", "-c", cmd.c_str(), (char *)nullptr);
            _exit(127);
        }

        // parent
        pid_        = pid;
        pid_public_ = static_cast<long long>(pid_);
        out_rd_     = out_pipe[0];
        out_wr_     = out_pipe[1];
        err_rd_     = err_pipe[0];
        err_wr_     = err_pipe[1];
        // parent doesn't write
        ::close(out_wr_);
        out_wr_ = -1;
        ::close(err_wr_);
        err_wr_ = -1;

        // set non-blocking reads (optional; our reader threads will still block in read()
        // but setting O_NONBLOCK helps in some shutdown races).
        set_nonblocking(out_rd_);
        set_nonblocking(err_rd_);

        running_ = true;

        t_out_ = libcxx::thread([capture_output, this]() {
            read_pipe_posix(out_rd_, /*is_stdout=*/true, capture_output);
        });
        t_err_ = libcxx::thread([capture_output, this]() {
            read_pipe_posix(err_rd_, /*is_stdout=*/false, capture_output);
        });
#endif
    }

#if !(defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64))
    static void set_nonblocking(int fd) {
        int flags = fcntl(fd, F_GETFL, 0);
        if (flags >= 0) {
            fcntl(fd, F_SETFL, flags | O_NONBLOCK);
        }
    }

    void read_pipe_posix(int fd, bool is_stdout, bool capture_output) {
        array<char, 4096> buf{};

        for (;;) {
            ssize_t n = ::read(fd, buf.data(), buf.size());

            if (n > 0) {
                if (is_stdout) {
                    output_.append_stdout(buf.data(), static_cast<size_t>(n));
                } else {
                    output_.append_stderr(buf.data(), static_cast<size_t>(n));
                }

                if (!capture_output) {  // if not capturing then emit to the terminal
                    if (is_stdout) {
                        libcxx::cout.write(buf.data(), static_cast<libcxx::streamsize>(n));
                        libcxx::cout.flush();
                    } else {
                        libcxx::cerr.write(buf.data(), static_cast<libcxx::streamsize>(n));
                        libcxx::cerr.flush();
                    }
                }

                continue;
            }

            if (n == 0) {
                break;  // eoF
            }

            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // small sleep to avoid busy spin
                libcxx::this_thread::sleep_for(libcxx::chrono::milliseconds(2));
                continue;
            }

            if (errno == EINTR) {
                continue;
            }

            // error; append to stderr buffer for visibility and exit
            if (!is_stdout) {
                const char *msg = "read() error on pipe\n";
                output_.append_stderr(msg, ::strlen(msg));
            }

            break;
        }

        if (fd >= 0) {
            ::close(fd);
        }

        is_stdout ? (out_rd_ = -1) : (err_rd_ = -1);
    }

    void blocking_waitpid() {
        int   status = 0;
        pid_t r      = ::waitpid(pid_, &status, 0);
        if (r == -1) {
            int e = errno;
            throw libcxx::runtime_error("waitpid failed: errno=" + libcxx::to_string(e));
        }
        finalize_posix(status, true);
    }

    void finalize_posix(int status, bool mark_terminated = false) {
        // ensure readers drained
        join_readers();

        if (WIFEXITED(status)) {
            return_code_ = WEXITSTATUS(status);
        } else if (WIFSIGNALED(status)) {
            // map signaled termination to negative signal for visibility
            return_code_ = -WTERMSIG(status);
        } else {
            return_code_ = -1;
        }
        was_terminated_ = was_terminated_ || mark_terminated;
        mark_finished();

        // close remaining fds if any
        if (out_rd_ >= 0) {
            ::close(out_rd_), out_rd_ = -1;
        }
        if (err_rd_ >= 0) {
            ::close(err_rd_), err_rd_ = -1;
        }
    }
#else
    void read_pipe_windows(HANDLE hPipe, bool is_stdout, bool capture_output) {
        array<char, 4096> buf{};
        DWORD             bytesRead = 0;
        for (;;) {
            BOOL ok =
                ReadFile(hPipe, buf.data(), static_cast<DWORD>(buf.size()), &bytesRead, nullptr);
            if (!ok) {
                DWORD err = GetLastError();
                if (err == ERROR_BROKEN_PIPE)
                    break;  // eoF
                // preserve partial results; raise error by terminating process reading
                // but we don't want to crash app; capture error into stderr stream.
                if (!is_stdout) {
                    output_.append_stderr("ReadFile error: ", 16);
                }
                break;
            }
            if (bytesRead == 0) {
                break;
            }
            if (is_stdout) {
                output_.append_stdout(buf.data(), bytesRead);
            } else {
                output_.append_stderr(buf.data(), bytesRead);
            }

            if (!capture_output) {  // if not capturing then emit to the terminal
                if (is_stdout) {
                    libcxx::cout.write(buf.data(), static_cast<libcxx::streamsize>(bytesRead));
                    libcxx::cout.flush();
                } else {
                    libcxx::cerr.write(buf.data(), static_cast<libcxx::streamsize>(bytesRead));
                    libcxx::cerr.flush();
                }
            }
        }
        // close our read handle when reader exits
        if (hPipe) {
            CloseHandle(hPipe);
        }
        if (is_stdout) {
            hStdoutRd_ = nullptr;
        } else {
            hStderrRd_ = nullptr;
        }
    }

    void finalize_windows(bool mark_terminated = false) {
        // ensure readers have consumed everything
        join_readers();

        DWORD exitCode = 0;
        if (!GetExitCodeProcess(pi_.hProcess, &exitCode)) {
            DWORD err = GetLastError();
            mark_finished();
            CloseHandle(pi_.hProcess);
            pi_.hProcess = nullptr;
            CloseHandle(pi_.hThread);
            pi_.hThread = nullptr;
            throw libcxx::runtime_error("GetExitCodeProcess failed! Error: " +
                                        libcxx::to_string(err));
        }
        return_code_    = static_cast<int>(exitCode);
        was_terminated_ = was_terminated_ || mark_terminated;
        mark_finished();

        if (pi_.hProcess) {
            CloseHandle(pi_.hProcess), pi_.hProcess = nullptr;
        }
        if (pi_.hThread) {
            CloseHandle(pi_.hThread), pi_.hThread = nullptr;
        }
    }
#endif
  private:
    using time_point = libcxx::chrono::steady_clock::time_point;

    // state
    int                 return_code_{-1};
    bool                timed_out_{false};
    bool                was_terminated_{false};
    long long           pid_public_{-1};
    time_point          start_time_;
    time_point          end_time_;
    ProcessOutput       output_;
    libcxx::atomic_bool running_{false};

    // reader threads
    libcxx::thread t_out_;
    libcxx::thread t_err_;

#if defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64)
    PROCESS_INFORMATION pi_{};
    HANDLE              hStdoutRd_{nullptr}, hStdoutWr_{nullptr};
    HANDLE              hStderrRd_{nullptr}, hStderrWr_{nullptr};
#else
    pid_t pid_{-1};
    int   out_rd_{-1}, out_wr_{-1};
    int   err_rd_{-1}, err_wr_{-1};
#endif
};

// convenience helper similar to your original signature but async-capable.
// returns a Subprocess you can wait on later.
// example usage:
//  auto p = async_system(L"ls -la", true, {{"PATH", "/usr/bin"}}, L"/home/user");
//  p.wait();  // or p.timeout(5000) to wait with timeout
//  auto output = p.output();  // get ProcessOutput with stdout/stderr data
//  if (output.return_code != 0) {
//      // handle error
//  } else {
//      // process output.stdout_data and output.stderr_data
//  }
inline Subprocess async_system(const string              &wcmd,
                               bool                       capture_output = false,
                               const map<string, string> &env            = {},
                               const string              *working_dir    = nullptr) {
    return Subprocess::run(wcmd, capture_output, env, working_dir);
}

// synchronous helper with timeout that returns unified ProcessOutput.
inline ProcessOutput system(const string              &wcmd,
                            bool                       capture_output = false,
                            int                        timeout_ms     = -1,
                            const map<string, string> &env            = {},
                            const string              *working_dir    = nullptr) {
    auto p = Subprocess::run(wcmd, capture_output, env, working_dir);
    return p.timeout(timeout_ms);
}

H_STD_NAMESPACE_END
H_NAMESPACE_END

#endif  // _$_HX_CORE_M6SYSTEM
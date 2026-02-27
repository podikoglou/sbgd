#include <algorithm>
#include <cctype>
#include <csignal>
#include <cstdio>
#include <cstring>
#include <dirent.h>
#include <filesystem>
#include <iterator>
#include <stdio.h>
#include <sys/wait.h>
#include <sysexits.h>
#include <unistd.h>
#include <vector>

namespace fs = std::filesystem;

const std::vector<std::string> ALLOWED_EXTENSIONS = {".png", ".jpg", ".jpeg"};

bool strings_eq_ignore_case(std::string a, std::string b) {
  if (a.length() != b.length())
    return false;

  auto it_a = a.begin();
  auto it_b = b.begin();

  while (it_a != a.end()) {
    if (std::tolower(*it_a) != std::tolower(*it_b))
      return false;

    it_a++;
    it_b++;
  }

  return true;
}

bool is_extension_allowed(std::string ext) {
  for (std::string ext2 : ALLOWED_EXTENSIONS) {
    if (strings_eq_ignore_case(ext, ext2)) {
      return true;
    }
  }

  return false;
}

std::vector<fs::path> read_walls(fs::path path) {
  fs::directory_iterator iter{path};
  std::vector<fs::path> paths;

  /* go through all files in the directory */
  for (auto entry : iter) {
    auto path = entry.path();
    auto ext = path.extension().string();

    /* ensure extension is allowed */
    if (!is_extension_allowed(ext)) {
      continue;
    }

    paths.push_back(path);
  }

  return paths;
}

std::vector<std::string> make_args(fs::path wall_path) {

  // TODO: maybe allow to pass some/all args
  std::vector<std::string> args{
      "swaybg",
      "--image",
      wall_path,
  };

  return args;
}

int main(int argc, char **argv) {
  /* validate arguments */
  if (argc < 2) {
    fprintf(stderr, "usage: %s <dir>\n", argv[0]);
    return EX_USAGE;
  }

  bool daemonize = argv[1][0] == '-' && argv[1][1] == 'd';
  fs::path path(daemonize ? argv[2] : argv[1]);

  if (daemonize) {
    int daemon_result;
    if ((daemon_result = daemon(0, 0)) != 0) {
      fprintf(stderr, "call to daemon() failed: %d\n", daemon_result);
      return EX_OSERR;
    }
  }

  // seed rng with time (should be fine)
  std::srand(std::time(0));

  while (true) {
    // read walls and choose a random one
    std::vector<fs::path> walls;
    try {
      walls = read_walls(path);
    } catch (const fs::filesystem_error &error) {
      fprintf(stderr, "%s\n", error.what());
      return EX_OSERR;
    }

    fs::path wall = walls[std::rand() % walls.size()];

    // fork the process
    __pid_t pid = fork();

    if (pid == 0) {
      // run swaybg in the child
      std::vector<std::string> args = make_args(wall);
      std::vector<char *> c_str_args;

      std::transform(args.begin(), args.end(), std::back_inserter(c_str_args),
                     [](std::string &arg) { return arg.data(); });
      c_str_args.push_back(nullptr);

      execvp("swaybg", c_str_args.data());
      perror("execvp");
      _exit(1);
    } else {
      // in this process, await a signal
      printf("child pid is: %d\n", pid);

      int status;
      printf("waiting for pid %d\n", pid);
      waitpid(pid, &status, 0);
    }
  }

  return EX_OK;
}

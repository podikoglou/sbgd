#include <cctype>
#include <csignal>
#include <cstdio>
#include <cstring>
#include <dirent.h>
#include <filesystem>
#include <stdio.h>
#include <sys/wait.h>
#include <sysexits.h>
#include <unistd.h>
#include <vector>

namespace fs = std::filesystem;

const std::vector<std::string> ALLOWED_EXTENSIONS = {".png", ".jpg", ".jpeg",
                                                     ".webp"};

// we use this special default value of -2 (-1 is reserved for errors by fork())
// to signify that we must start swaybg. this value is also used for when we are
// restarting it
__pid_t pid = -2;

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

int main(int argc, char **argv) {
  /* validate arguments */
  if (argc < 2) {
    fprintf(stderr, "usage: %s <dir>\n", argv[0]);
    return EX_USAGE;
  }

  fs::path path(argv[1]);

  // seed rng with time (should be fine)
  std::srand(std::time(0));

  while (true) {
    // read walls and choose a random one
    auto walls = read_walls(path);
    auto wall = walls[std::rand() % walls.size()];

    // fork the process
    pid = fork();

    if (pid == 0) {
      // run swaybg in the child

      // maybe allow to pass some/all args
      char *args[] = {strdup("swaybg"), strdup("--image"), strdup(wall.c_str()),
                      nullptr};

      execvp("swaybg", args);
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

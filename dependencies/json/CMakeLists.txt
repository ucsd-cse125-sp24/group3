include(FetchContent)

  # orignally used the snippet from here
  # https://github.com/ArthurSonzogni/nlohmann_json_cmake_fetchcontent
  # but this has a bug in MSVS, so I used the line from this GitHub issue instead
  # https://github.com/owodzeg/V4Hero/issues/91
FetchContent_Declare(json GIT_REPOSITORY https://github.com/nlohmann/json GIT_TAG v3.11.2)

FetchContent_MakeAvailable(json)
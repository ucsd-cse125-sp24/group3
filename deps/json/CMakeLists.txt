  include(FetchContent)

  # https://github.com/ArthurSonzogni/nlohmann_json_cmake_fetchcontent
  FetchContent_Declare(json
    GIT_REPOSITORY https://github.com/ArthurSonzogni/nlohmann_json_cmake_fetchcontent
    
    URL_HASH SHA256=282446b30a285ea593ee7baaf6fb8978551fd3671e78b1b869270a62d1decf3b
    DOWNLOAD_EXTRACT_TIMESTAMP true
    GIT_PROGRESS TRUE
    GIT_SHALLOW TRUE
    GIT_TAG v3.11.2)

  FetchContent_MakeAvailable(json)
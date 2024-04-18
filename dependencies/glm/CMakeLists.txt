include(FetchContent)

FetchContent_Declare(glm 
    URL https://github.com/g-truc/glm/archive/refs/tags/1.0.1.zip
    URL_HASH SHA256=09c5716296787e1f7fcb87b1cbdbf26814ec1288ed6259ccd30d5d9795809fa5
    DOWNLOAD_EXTRACT_TIMESTAMP true
)

FetchContent_MakeAvailable(glm)

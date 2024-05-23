#include <gtest/gtest.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include "client/util.hpp"

TEST(Bbox, BboxCombine) {
    struct TestCase {
        std::string name;
        
        Bbox bbox1;
        Bbox bbox2;
        Bbox expectedCombined;
    };
    
    const std::vector<TestCase> testCases{{
        {
            .name = "two 1 unit boxes",
            .bbox1 = Bbox {
                .corners = std::pair<glm::vec3, glm::vec3>(
                    glm::vec3(-1.0f, -1.0f, -1.0f),
                    glm::vec3(0.0f, 0.0f, 0.0f)
                )
            },
            .bbox2 = Bbox {
                .corners = std::pair<glm::vec3, glm::vec3>(
                    glm::vec3(0.0f, 0.0f, 0.0f),
                    glm::vec3(1.0f, 1.0f, 1.0f)
                )
            },
            .expectedCombined = Bbox {
                .corners = std::pair<glm::vec3, glm::vec3>(
                    glm::vec3(-1.0f, -1.0f, -1.0f),
                    glm::vec3(1.0f, 1.0f, 1.0f)
                )
            }
        },
        {
            .name = "one small and one large",
            .bbox1 = Bbox {
                .corners = std::pair<glm::vec3, glm::vec3>(
                    glm::vec3(-100.0f, -78.0f, -69.0f),
                    glm::vec3(-30.0f, -42.0f, -13.0f)
                )
            },
            .bbox2 = Bbox {
                .corners = std::pair<glm::vec3, glm::vec3>(
                    glm::vec3(10.0f, 10.0f, 10.0f),
                    glm::vec3(11.0f, 11.0f, 11.0f)
                )
            },
            .expectedCombined = Bbox {
                .corners = std::pair<glm::vec3, glm::vec3>(
                    glm::vec3(-100.0f, -78.0f, -69.0f),
                    glm::vec3(11.0f, 11.0f, 11.0f)
                )
            }
        },
        {
            .name = "overlapping",
            .bbox1 = Bbox {
                .corners = std::pair<glm::vec3, glm::vec3>(
                    glm::vec3(5.0f, 5.0f, 5.0f),
                    glm::vec3(10.0f, 10.0f, 10.0f)
                )
            },
            .bbox2 = Bbox {
                .corners = std::pair<glm::vec3, glm::vec3>(
                    glm::vec3(7.5f, 7.5f, 7.5f),
                    glm::vec3(15.0f, 15.0f, 15.0f)
                )
            },
            .expectedCombined = Bbox {
                .corners = std::pair<glm::vec3, glm::vec3>(
                    glm::vec3(5.0f, 5.0f, 5.0f),
                    glm::vec3(15.0f, 15.0f, 15.0f)
                )
            }
        },
        {
            .name = "one inside another",
            .bbox1 = Bbox {
                .corners = std::pair<glm::vec3, glm::vec3>(
                    glm::vec3(0.0f, 0.0f, 0.0f),
                    glm::vec3(10.0f, 10.0f, 10.0f)
                )
            },
            .bbox2 = Bbox {
                .corners = std::pair<glm::vec3, glm::vec3>(
                    glm::vec3(0.01f, 0.01f, 0.01f),
                    glm::vec3(9.99f, 9.99f, 9.99f)
                )
            },
            .expectedCombined = Bbox {
                .corners = std::pair<glm::vec3, glm::vec3>(
                    glm::vec3(0.0f, 0.0f, 0.0f),
                    glm::vec3(10.0f, 10.0f, 10.0f)
                )
            }
        },
        {
            .name = "another test",
            .bbox1 = Bbox {
                .corners = std::pair<glm::vec3, glm::vec3>(
                    glm::vec3(-3.0f, 4.0f, -5.0f),
                    glm::vec3(10.0f, -2.0f, 5.0f)
                )
            },
            .bbox2 = Bbox {
                .corners = std::pair<glm::vec3, glm::vec3>(
                    glm::vec3(1.0f, -10.0f, 20.0f),
                    glm::vec3(-100.0f, 6.0f, -10.0f)
                )
            },
            .expectedCombined = Bbox {
                .corners = std::pair<glm::vec3, glm::vec3>(
                    glm::vec3(-100.0f, -10.0f, -10.0f),
                    glm::vec3(10.0f, 6.0f, 20.0f)
                )
            }
        },
    }};

    for (const auto &tc : testCases) {
        Bbox combined = combineBboxes(tc.bbox1, tc.bbox2);
        // checking string equality because it's easier to see the differences 
        EXPECT_EQ(glm::to_string(combined.corners.first), glm::to_string(tc.expectedCombined.corners.first));
        EXPECT_EQ(glm::to_string(combined.corners.second), glm::to_string(tc.expectedCombined.corners.second));
    };
}

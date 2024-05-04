cc_library(name="mutable",
           hdrs=["mutable.h"],
           srcs=["mutable.cc"],
           deps=["//boost:di"])

cc_test(name="mutable_test",
           srcs=["mutable_test.cc"],
           deps=[":mutable",
                 "@googletest//:gtest_main"])

cc_library(name="behavior_tree",
           srcs=["behavior_tree.cc"],
           hdrs=["behavior_tree.h"],
           deps=["//boost:di",
                 ":mutable"])

cc_library(name="tickles",
           deps=[":mutable",
                 ":behavior_tree"])

cc_test(name="behavior_tree_test",
        srcs=["behavior_tree_test.cc"],
        deps=[":behavior_tree",
	      "@googletest//:gtest_main",
              "//boost:di"],)

cc_test(name="robot_test",
        srcs=["robot.cc"],
        deps=[":node", "@googletest//:gtest_main"])

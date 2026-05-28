/// --- The Kairo Project -------------------------------------------------- ///
///
///   Part of the Kairo Project, under the Apache License v2.0 with the
///   Kairo Runtime Library Exception.
///
///   See: https://www.kairolang.org/LICENSE.txt
///   SPDX-License-Identifier: Apache-2.0 WITH KAIRO-RUNTIME-EXCEPTION
///   Copyright (c) 2026 Dhruvan Kartik
///
/// ------------------------------------------------------------------------ ///

import std::string::{clean} as clean;

print(clean("
    #===-------------- This is the std lib test file, this shall not be imported. --------------===#
    #                                                                                              #
    #                            ██╗  ██╗ █████╗ ██╗██████╗  ██████╗                               #
    #                            ██║ ██╔╝██╔══██╗██║██╔══██╗██╔═══██╗                              #
    #                            █████╔╝ ███████║██║██████╔╝██║   ██║                              #
    #                            ██╔═██╗ ██╔══██║██║██╔══██╗██║   ██║                              #
    #                            ██║  ██╗██║  ██║██║██║  ██║╚██████╔╝                              #
    #                            ╚═╝  ╚═╝╚═╝  ╚═╝╚═╝╚═╝  ╚═╝ ╚═════╝                               #
    #                                                                                              #
    #    Part of the Kairo Project, under the Apache License v2.0 with the                         #
    #    Kairo Runtime Library Exception.                                                          #
    #                                                                                              #
    #    See: https://www.kairolang.org/LICENSE.txt                                                #
    #    SPDX-License-Identifier: Apache-2.0 WITH KAIRO-RUNTIME-EXCEPTION                          #
    #    Copyright (c) 2026 Dhruvan Kartik                                                         #
    #                                                                                              #
    #===----------------------------------------------------------------------------------------===#
"));

test "lists" {
    define p_list! {
        print(li);
    };

    var li: list<int> = [1, 2, 3, 4, 5, 6];
    p_list!

    print(sizeof!(li));

    li.append(1);
    p_list!

    li += 2;

    var li2 = [1, 2, 3];
    li = li + li2;

    p_list!
}
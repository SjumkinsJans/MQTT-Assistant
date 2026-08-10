The idea is such that plugins, i.e. commands, can be added to the assistant
without the need for recompiling the whole thing after adding a new plugin.

Solution is trivial : each plugin itself has an already pre-compiled binary,
which ,akin to library , gets loaded into the programm, with only difference
being that it is loaded dynamically, using dlfcn library, which provides
dynamic linking interface.

Current folder structure :
(names are placeholders)

commands
        |\_command_control.c
        |\_command_control.h
        |
        |
        |\_plugin1
        |        \_plugin1.txt
        |        \_plugin1.c
        |        \_plugin1.so
        |
        |\_plugin2
        |        \_plugin2.txt
        |        \_plugin2.c
        |        \_plugin2.so


* - mandatory file
*plugin.txt - different words/word combinations that can be used to call the command (for example, command hello() could be called by typing "hello" AND "good morning", and in both cases aforementioned command would execute)
*plugin.os - binary file containing the code of the plugin in executable machine code.
plugin.c - is optional for the user, but very handy for the developer. Contains yet uncompiled C code. Can be adjusted and then compiled into plugin.so


Current plugin structure :

pluginABC.c:

    #include "command_control.h"
    int pluginABC(char * payload) {
        // get information from payload
        // execute command
        // return 0
    }

    // defined assisting functions if needed
            
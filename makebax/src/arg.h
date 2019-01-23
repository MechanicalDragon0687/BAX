#pragma once

#include <vector>

typedef std::pair<unsigned char, const char*> Argument;
typedef std::vector<Argument> ArgList;

static inline bool arg_isl(int c) {
	return (c >= 'a') && (c <= 'z');
}

static inline void ParseArguments(ArgList &l, int argc, char *argv[]) {
	const char *arg;
	int i = 0;

	while(i < argc) {
		unsigned char c;
		const char *param;

		arg = argv[i++];
		if (arg == nullptr)
			break;

		if (arg[0] != '-' || !arg_isl(arg[1]))
			continue;

		c = arg[1];

		if (i < argc) {
			arg = argv[i];
			if (arg && arg[0] != '\0' && arg[0] != '-') {
				param = arg;
				i++;
			} else {
				param = "";
			}
		} else {
			param = "";
		}

		l.push_back({c, param});
	}
}

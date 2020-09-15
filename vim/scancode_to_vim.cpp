#include "scancode_to_vim.h"

Map<uint32_t, VimAction> ScancodeToVim::map;

void ScancodeToVim::init() {
	const VimFunc INPUT = vimInput;
	const VimFunc KEY = vimKey;

	map[KEY_ESCAPE] = VimAction(KEY, "<esc>", false);
	map[KEY_LEFT] = VimAction(KEY, "<left>");
	map[KEY_RIGHT] = VimAction(KEY, "<right>");
	map[KEY_UP] = VimAction(KEY, "<up>");
	map[KEY_DOWN] = VimAction(KEY, "<down>");
	map[KEY_A] = VimAction(INPUT, "a", "A");
	map[KEY_C] = VimAction(INPUT, "c", "C");
	map[KEY_H] = VimAction(INPUT, "h", "H");
	map[KEY_I] = VimAction(INPUT, "i", "I");
	map[KEY_J] = VimAction(INPUT, "j", "J");
	map[KEY_K] = VimAction(INPUT, "k", "K");
	map[KEY_L] = VimAction(INPUT, "l", "L");
	map[KEY_R] = VimAction(INPUT, "r", "R");
	map[KEY_U] = VimAction(INPUT, "u", "U");
	map[KEY_W] = VimAction(INPUT, "w", "W");
}

bool ScancodeToVim::has_scancode(uint32_t scancode) {
	if (map.empty()) {
		init();
	}

	return map.has(scancode);
}

VimAction ScancodeToVim::get_vim_action(uint32_t scancode) {
	if (map.empty()) {
		init();
	}

	return map[scancode];
}

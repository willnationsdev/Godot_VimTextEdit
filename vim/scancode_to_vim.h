#ifndef SCANCODE_TO_VIM
#define SCANCODE_TO_VIM

#include "core/map.h"
#include "core/os/keyboard.h"
#include "_libvim.h"

typedef void (*VimFunc)(char_u*);

struct VimAction {
	/**
	 * The vim func to call with this key, either vimInput or vimKey
	 */
	VimFunc func;

	/**
	 * the argument to pass to the vim function if shift is not being held down
	 */
	char_u *argument;

	/**
	 * the argument to pass to the vim function if shift is being held down
	 */
	char_u *shift_argument;

	/**
	 * if this is true, then vim fully handled the key event, no more processing needed.
	 * if false, allow TextEdit to keep working with the event.
	 *
	 * For example, pressing esc should exit insert mode and close the autocomplete popup
	 */
	bool is_complete;

	VimAction():
		func(NULL),
		argument(NULL),
		shift_argument(NULL),
		is_complete(true)
	{}

	VimAction(VimFunc f, char_u* a, char_u* sa, bool com):
		func(f),
		argument(a),
		shift_argument(sa),
		is_complete(com)
	{}

	VimAction(VimFunc f, char_u* a, char_u* sa):
		func(f),
		argument(a),
		shift_argument(sa),
		is_complete(true)
	{}

	VimAction(VimFunc f, char_u* a):
		func(f),
		argument(a),
		shift_argument(a),
		is_complete(true)
	{}

	VimAction(VimFunc f, char_u* a, bool com):
		func(f),
		argument(a),
		shift_argument(a),
		is_complete(com)
	{}

};

class ScancodeToVim {
	static Map<uint32_t, VimAction> map;
	static void init();

public:
	static bool has_scancode(uint32_t scancode);
	static VimAction get_vim_action(uint32_t scancode);
};

#endif

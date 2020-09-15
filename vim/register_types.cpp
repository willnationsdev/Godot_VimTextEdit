
#include "register_types.h"

#include "core/class_db.h"
#include "text_edit.h"

#include "_libvim.h"


void register_vim_types() {
	vimInit(0, {});
	ClassDB::register_class<TextEdit>();
}

void unregister_vim_types() {
	// Nothing to do here in this example.
}

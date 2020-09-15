# Godot 3.2 Vim TextEdit

This is an early proof of concept of adding vim support to Godot's TextEdit control via [libvim](https://github.com/onivim/libvim).

This means when editing scripts inside Godot's editor, it is done using vim commands.

## Status

At time of writing, about 8 hours of hacking has been put into this proof of concept.

The code is _really_ messy.

I have never worked on Godot before, and have not coded in C or C++ in over a decade. That makes the code that much worse.

### These vim features are working

* navigate with `hjkl` in normal mode
* press `i` to enter insert mode
* press `esc` to return to normal mode
* `ciw` text object motion
* `u` for undo
* `shift+r` for redo
* when in insert mode, you can type to add characters to the file, but only a handful of keys are recognized
	* see `scancode_to_vim.cpp`

## The killer feature in all of this

But the cool thing is, the above features are being performed by vim itself. I just send the key events to vim and listen to updates it sends back. Getting all of vim's features is just a matter of finishing the integration with libvim. We don't have to painstakingly implement vim emulation in Godot.

## How it works

### Initializing vim and setting up the buffer

1. in `register_vim_types()` (register_types.cpp), `ClassDB::register_class<TextEdit>()` is called. This removes Godot's built in `TextEdit` control and replaces it with this vim based one.
  * also `vimInit()` is called here to get vim up and running
2. In TextEdit's `set_text` method (text_edit.cpp):
	* a vim buffer is created with `vimBufferOpen()`
	* the contents of the text for the file are sent to vim, one line at a time, with `vimBufferSetLines`
		* `vimBufferSetLines` can accept all the lines at once, but I had a lot of problems with that.

This means both Godot and vim have their own buffers of the file. Vim's buffer is only ever in memory, as we never tell it to save to disk.

### Sending commands to vim

1. in `_gui_input`, during the keyboard event handling, vim is given the first chance to respond to the event
	* search for `ScancodeToVim::has_scancode` to find the start of this code block
  * if the scancode is defined in `scancode_to_vim`'s map, then vim will get the command either via `vimInput()` or `vimKey()`
2. if vim received a command, we ask vim for the current cursor line and column, to update Godot's `cursor` struct accordingly (see `set_cursor_position_from_vim()`)

### updating the buffer

libvim informs us of buffer changes with a BufferUpdateCallback. This is set in `TextEdit`'s constructor by calling `vimSetBufferUpdateCallback()`. Since you can't easily pass a C++ member function as a C callback, the static function `TextEdit::vim_buffer_update` is used for this. Whenever a `TextEdit` is created, it registers itself in `vimCallbackMap`, which allows the static callback function to hand the update off to the correct `TextEdit`.

Inside `on_vim_buffer_update` (non static method, this is the TextEdit handling the vim update), we ask vim for all of the lines that have changed. We remove those lines from `TextEdit`'s text field via `_remove_text`, then insert the newly updated lines with `_insert_text`. The new lines are obtained with `vimBufferGetLine()`.

### the scancode map

This is strange and could be much better. Whenever a key is pressed, we need to send that key to vim. This map holds mappings from Godot scancode values to a struct containing the info we need to send the corresponding vim command. If all scancodes were in the map (ie the entire alphabet, all numbers, symbols, etc), then that would mean vim could handle just about any command.

I see `InputEventKey` also has a `unicode` property. So possibly we just need to conver that to a char and send it to vim?

# known issues

there are ton. this is terribly hacked together and raw as can be.

## TextEdit largely remains intact

The vim functionality was jammed in as quickly as possible. To do this properly, the vim version of `TextEdit` will likely need to change _a lot_. However, this would also be a good chance to understand what kind of api hooks and support Godot could provide to make adding vim easy for all parties involved (the vim editor devs who would maintain a plugin like this, and the core Godot devs who would stick to working on Godot itself).

## vim modes

There is no visual indication of what mode you are in.

## visual mode and more advanced vim features

not implemented at all. visual mode in particular will require some coordination between vim and TextEdit.


# Pros and Cons

## Pros

* libvim makes it pretty easy to get a lot of vim functionality for very little effort.
* syntax highlighting, autocomplete popups, can still work as implemented
* since this is a module, if it was taken all the way and made into a real piece of code, those who want vim in Godot could get it without bothering anyone else
	* it may be possible to do this via GDNative as well

## Cons

* Godot's `TextEdit` control gets completely replaced. As Godot stands now, there is no other way to do this. The biggest gotcha to this is if a game uses a TextEdit, that game's TextEdit would be the vim version, which is probably really unexpected.
* keeping Godot's text buffer and vim's text buffer in sync might prove to be a pain
* the entirety of vim is compiled into `libvim.a`
* libvim is a young library and probably needs some hardening.

# How to install

NOTE: this entire repo currently assumes you are using Linux. I am using Ubuntu 18.04.

## build libvim

Instructions taken from [libvim](https://github.com/onivim/libvim)

1. install esy: `npm install -g esy@0.5.7`
2. `git clone https://github.com/onivim/libvim`
3. `cd src`
4. `esy install`
5. `esy '@test' install` (possibly not needed)
6. `esy build`

## copy libvim over into the module

NOTE: for the time being, this repo has the built artifacts checked in. if you are on Ubuntu 18, you can use them as is and skip all of this.

There is surely a better way to do this. But this got the job done for now.

1. copy libvim.a from `src/_esy/default/store/b/libvim-b6c18b56/libvim.a` in the libvim tree over into `Godot_VimTextEdit/vim/lib`.
2. copy vim's headers over: `cp src/_esy/default/store/b/libvim-db9de2ae/*.h ../Godot_VimTextEdit/vim/inc`
3. copy vim's proto headers over: `cp -R src/_esy/default/store/b/libvim-b6c18b56/auto ../Godot_VimTextEdit/vim/inc/auto`

NOTE: the sha on the end of the `libvim` folder may be different for you

## Build Godot with this module

1. `git clone https://github.com/godotengine/godot.git`
2. `git checkout 3.2`
3. `scons custom_modules="../Godot_VimTextEdit" platform=x11`
	* takes about 15 minutes on my machine
	* vim will spit out a lot of warnings

If you are having trouble, Godot has great docs on [how to compile it](https://docs.godotengine.org/en/stable/development/compiling/index.html)

## Launch Godot

1. `bin/godot*`
2. edit a script file



/*************************************************************************/
/*  export_plugin.cpp                                                    */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2022 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2022 Godot Engine contributors (cf. AUTHORS.md).   */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/

#include "export_plugin.h"

#include "core/config/project_settings.h"
#include "editor/editor_node.h"

Error EditorExportPlatformLinuxBSD::_export_debug_script(const Ref<EditorExportPreset> &p_preset, const String &p_app_name, const String &p_pkg_name, const String &p_path) {
	FileAccessRef f = FileAccess::open(p_path, FileAccess::WRITE);
	ERR_FAIL_COND_V(!f, ERR_CANT_CREATE);

	f->store_line("#!/bin/sh");
	f->store_line("echo -ne '\\033c\\033]0;" + p_app_name + "\\a'");
	f->store_line("base_path=\"$(dirname \"$(realpath \"$0\")\")\"");
	f->store_line("\"$base_path/" + p_pkg_name + "\" \"$@\"");

	return OK;
}

Error EditorExportPlatformLinuxBSD::export_project(const Ref<EditorExportPreset> &p_preset, bool p_debug, const String &p_path, int p_flags) {
	Error err = EditorExportPlatformPC::export_project(p_preset, p_debug, p_path, p_flags);

	if (err != OK) {
		return err;
	}

	String app_name;
	if (String(ProjectSettings::get_singleton()->get("application/config/name")) != "") {
		app_name = String(ProjectSettings::get_singleton()->get("application/config/name"));
	} else {
		app_name = "Unnamed";
	}
	app_name = OS::get_singleton()->get_safe_dir_name(app_name);

	// Save console script.
	if (err == OK) {
		int con_scr = p_preset->get("debug/export_console_script");
		if ((con_scr == 1 && p_debug) || (con_scr == 2)) {
			String scr_path = p_path.get_basename() + ".sh";
			err = _export_debug_script(p_preset, app_name, p_path.get_file(), scr_path);
			FileAccess::set_unix_permissions(scr_path, 0755);
		}
	}

	return err;
}

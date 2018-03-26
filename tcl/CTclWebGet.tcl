proc CTclWebGetCreateWidgets { parent } {
  set menu_frame $parent.menu_frame

  frame $menu_frame

  set menubar $parent.menubar

  menu $menubar

  $parent. config -menu $menubar

  set file_menu [CTclAppMenuAdd $parent.menubar "File" 0]

  CTclAppMenuAddCmd "Exit" CTclWebGetExit 0

  set help_menu [CTclAppMenuAdd $parent.menubar "Help" 0]

  CTclAppMenuAddCmd "Help" CTclWebGetHelp 0

  pack $menu_frame

  #---

  set url_frame $parent.url_frame

  frame $url_frame

  label $url_frame.label -text "Url"
  entry $url_frame.entry -textvariable ctcl_webget_url_value -width 48

  CTclAppAddReturnTextBinding $url_frame.entry CTclWebGetAddUrlCmd

  pack $url_frame.label -side left -padx 4
  pack $url_frame.entry -side left

  pack $url_frame -side top -anchor w

  #---

  set tree_frame $parent.tree_frame

  frame $tree_frame

  global ctcl_webget_tree_widget
  global ctcl_webget_tree_hlist

  set ctcl_webget_tree_widget $tree_frame.tree_widget

  tixTree $ctcl_webget_tree_widget -width 300

  set ctcl_webget_tree_hlist [$ctcl_webget_tree_widget subwidget hlist]

  $ctcl_webget_tree_hlist configure -separator |

  $ctcl_webget_tree_hlist configure -browsecmd CTclWebGetBrowseTreeEntry

  pack $ctcl_webget_tree_widget -side top -expand true -fill both

  pack $tree_frame -side top -expand true -fill both
}

proc CTclWebGetAddUrlCmd { } {
  global ctcl_webget_url_value

  CTclWebGetAddUrl $ctcl_webget_url_value
}

proc CTclWebGetBrowseTreeEntry { entry } {
  global ctcl_webget_tree_hlist

  set url [$ctcl_webget_tree_hlist info data $entry]

  if {$url != ""} {
    global ctcl_webget_url_value

    set ctcl_webget_url_value $url
  }
}

proc CTclWebGetAddTreeEntry { path name data has_children loaded } {
  global ctcl_webget_tree_hlist

  if {! [$ctcl_webget_tree_hlist info exists $path]} {
    $ctcl_webget_tree_hlist add $path -text $name -data $data
  }

  if {$has_children} {
    global ctcl_webget_tree_widget

    $ctcl_webget_tree_widget setmode $path close
  }
}

CTclWebGetCreateWidgets ""

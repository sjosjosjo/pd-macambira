#!/usr/bin/env tclsh
# Polish (polski) translations for PureData
# $Id: polski.tcl,v 1.1.2.2 2007-08-01 04:24:43 matju Exp $
# by Michal Seta, mis@artengine.ca

say file "Plik"
  say new_file "Nowy Plik"
  say open_file "Otwórz Plik..."
  say server_prefs "Preferencje servera..."
  say client_prefs "Preferencje klienta..."
  say send_message "Wysłać Wiadomość..."
  say paths "Ścieżki..."
  say close "Zamknąć"
  say save "Zapisz"
  say save_as "Zapisz Jako..."
  say print "Wydrukuj..."
  say quit "Zakończ"
  
  say canvasnew_file "Nowy Plik"
  say canvasopen_file "Otwórz Plik..."
  say canvassave "Zapisz"
  say canvassave_as "Zapisz Jako..."
  say clientpdrc_editor "Edycja .pdrc"
  say clientddrc_editor "Edycja .ddrc"
  say canvasclose "Zamknij"
  say canvasquit "Zakończ"
  
say edit "Edycja"
  say undo "Cofnij"
  say redo "Ponów"
  say cut "Wytnij"
  say copy "Skopiuj"
  say paste "Wklei"
  say duplicate "Powiel"
  say select_all "Zaznacz wszystko"
  say text_editor "Edytor tekstu..."
  say font "Czcionka"
  say tidy_up "Wyrównać"
  say edit_mode "Tryb edycji"
  say editmodeswitch "Tryb edycji/pracy"
  
  say canvascut "Wytnij"
  say canvascopy "Skopiuj"
  say canvasundo "Anuluj"
  say canvasredo "Ponów"
  say canvaspaste "Wklei"
  say canvasduplicate "Powiel"
  say canvasselect_all "Zaznacz wszystko"
  say canvaseditmodeswitch "Tryb edycji/pracy"
  
say view "Widok"
  say reload "Załaduj ponownie"
  say redraw "Odśwież"
  
  say canvasreload "Załaduj ponownie"
  say canvasredraw "Odśwież"
  
  say find "Szukaj"
  say find_again "Szukaj ponownie"
  say find_last_error "Znajdź ostatni błąd"
   say string "Znajdź ciąg znaków"
 say canvasfind "Szukaj"
  say canvasfind_again "Szukaj ponownie"

say put "Połóż"
  say Object "Objekt"
  say Message "Komunikat"
  say Number "Liczba"
  say Symbol "Znak"
  say Comment "Komentarz"
  say Array "Tabela"

say media "Media"
  say audio_on "Włącz dźwięk"
  say audio_off "Wyłącz dźwięk"
  say test_audio_and_midi "Próba dżwięku i MIDI"
  say load_meter "Miernik procesora"
  
  say canvasaudio_on "Włącz dźwięk"
  say canvasaudio_off "Wyłącz dźwięk"
  say clienttest_audio_and_midi "Próba dżwięku i MIDI"
  say canvasload_meter "Miernik procesora"

say window "Okno"

say help "Pomoc"
  say about "Na temat..."
  say pure_documentation "Dokumentacja..."
  say class_browser "Przeglądarka klas..."

  say canvasabout "Na temat..."
  
  say properties "Właściwości"
say open "Otwórz"

### for key binding editor
say general "Ogólne"
say audio_settings "Ustawienia dżwięku"
say midi_settings "Ustawienia MIDI"
say latency_meter "Miernik opóźnienia"
say Pdwindow "Okno PD"

say canvaspdwindow "Konsola PD"
say canvaslatency_meter "Miernik opóźnienia"
say clientaudio_settings "Ustawienia dżwięku"
say clientmidi_settings "Ustawienia MIDI"

### for Properties Dialog (phase 5B)
say_category IEM
say w "Szerokość(px)"
say h "Wysokość(px)"
say hold "Czas utrzymania(ms)"
say break "Czas zatrzymania(ms)"
say min "Zmienna minimalna"
say max "Zmienna maxymalna"
say is_log "Tryb"
say linear "Liniowy"
say logarithmic "Logarytmiczny"
say isa "Uruchomienie"
say n "Ilość wyboru"
say steady "Równomierny"
say steady_no  "Skok po kliknięciu"
say steady_yes "Stały po kliknięciu"
say snd "oznakowanie wysłania"
say rcv "oznakowanie pobrania"
say lab "etykieta"
say ldx "przesunięcie etykiety po x"
say ldy "przesunięcie etykiety po y"
say fstyle "Czcionka"
say fs "Rozmiar czcionki"
say bcol "Kolor tła"
say fcol "Kolor przedni"
say lcol "Kolor etykiety"
say yes "Tak"
say no "Nie"
say courier "courrier (typewriter)"
say helvetica "helvetique (sansserif)"
say times "times (serif)"
say coords "grafika na nadrzędnym"

say_category GAtomProperties
say width "szerokość"
say lo "ograniczenie niskie"
say hi "ograniczenie wysole"
say label "etykieta"
say wherelabel "wyświetlić etykietę"
say symto "wyślij znak"
say symfrom "pobierz znak"

say_category GraphProperties
say x1   "x od"
say x2   "x do"
say xpix "szerokość ekranu"
say y2   "y od"
say y1   "y do"
say ypix "wysokość ekranu"

say_category CanvasProperties
#say xscale "X units/px"
#say yscale "Y units/px"
say gop "grafika na nadrzędnym"
say xmargin "margines x"
say ymargin "margines y"
say height "wysokość"
say_category ArrayProperties
say name "nazwa"
say n    "rozmiar"
say xfrom "zakres x od"
say xto "zakres x do"
say yfrom "zakres y od"
say yto "zakres y do"


say_category MainWindow
say in "wejście"
say out "wyjście"
say audio "Dźwięk"
say meters "Pomiary"
say io_errors "Błędy I/O"
say tcl_console "Klient Tcl"
say pd_console "Server pd"

### phase 2

say_category Other
say_namespace summary {
  say_category IEMGUI
  say bng    "Bang"
  say tgl    "Przerzutnik"
  say nbx    "Liczba"
  say hsl    "Suwak (poziomy)"
  say vsl    "Suwak (Pionowy)"
  say hradio "Pole wyboru (poziome)"
  say vradio "Pole wyboru (pionowe)"
  say cnv    "Płótno"
  say dropper "Drag-and-Drop Box"
  say vu     "Miernik VU"

  say_category GLUE
  say bang "output a bang message"
  say float "store and recall a number"
  say symbol "store and recall a symbol"
  say int "store and recall an integer"
  say send "send a message to a named object"
  say receive "catch sent messages"
  say select "test for matching numbers or symbols"
  say route "route messages according to first element"
  say pack "make compound messages"
  say unpack "get elements of compound messages"
  say trigger "sequence and convert messagess"
  say spigot "interruptible message connection"
  say moses "part a numeric stream"
  say until "looping mechanism"
  say print "print out messages"
  say makefilename "format a symbol with a variable field"
  say change "remove repeated numbers from a stream"
  say swap "swap two numbers"
  say value "shared numeric value"

  say_category TIME
  say delay "send a message after a time delay"
  say metro "send a message periodically"
  say line "send a series of linearly stepped numbers"
  say timer "measure time intervals"
  say cputime "measure CPU time"
  say realtime "measure real time"
  say pipe "dynamically growable delay line for numbers"

  say_category MATH
  say + "add"
  say - "substract"
  say * "multiply"
  say {/ div} "divide"
  say {% mod} "division remainder"
  say pow "exponentiate"
  say == "equal?"
  say != "not equal?"
  say > "more than?"
  say < "less than?"
  say >= "not less than?"
  say <= "not more than?"
  say &  "bitwise conjunction (and)"
  say |  "bitwise disjunction (or)"
  say && "logical conjunction (and)"
  say || "logical disjunction (or)"
  say mtof "MIDI to Hertz"
  say ftom "Hertz to MIDI"
  say powtodb "Watts to dB"
  say dbtopow "dB to Watts"
  say rmstodb "Volts to dB"
  say dbtorms "dB to Volts"
  say {sin cos tan atan atan2 sqrt} "trigonometry"
  say log "Euler logarithm"
  say exp "Euler exponential"
  say abs "absolute value"
  say random "random"
  say max "greater of two numbers"
  say min "lesser of two numbers"
  say clip "force a number into a range"

  say_category MIDI
  say {notein ctlin pgmin bendin touchin polytouchin midiin sysexin} "MIDI input"
  say {noteout ctlout pgmout bendout touchout polytouchout midiout}  "MIDI output"
  say makenote "schedule a delayed \"note off\" message corresponding to a note-on"
  say stripnote "strip \"note off\" messages"

  say_category TABLES
  say tabread "read a number from a table"
  say tabread4 "read a number from a table, with 4 point interpolation"
  say tabwrite "write a number to a table"
  say soundfiler "read and write tables to soundfiles"

  say_category MISC
  say loadbang "bang on load"
  say serial "serial device control for NT only"
  say netsend "send messages over the internet"
  say netreceive "receive them"
  say qlist "message sequencer"
  say textfile "file to message converter"
  say openpanel "\"Open\" dialog"
  say savepanel "\"Save as\" dialog"
  say bag "set of numbers"
  say poly "polyphonic voice allocation"
  say {key keyup} "numeric key values from keyboard"
  say keyname "symbolic key name"

  say_category "AUDIO MATH"
  foreach word {+ - * /} {say $word~ "[say $word] (for signals)"}
  say max~ "supremum of signals"
  say min~  "infimum of signals"
  say clip~ "constrict signal to lie between two bounds"
  say q8_rsqrt~ "cheap reciprocal square root (beware -- 8 bits!)"
  say q8_sqrt~ "cheap square root (beware -- 8 bits!)"
  say wrap~ "wraparound (fractional part, sort of)"
  say fft~ "complex forward discrete Fourier transform"
  say ifft~ "complex inverse discrete Fourier transform"
  say rfft~ "real forward discrete Fourier transform"
  say rifft~ "real inverse discrete Fourier transform"
  say framp~ "output a ramp for each block"
  foreach word {mtof ftom rmstodb dbtorms rmstopow powtorms} {
    say $word~ "[say $word] (for signals)"
  }
}

### phase 3

say_namespace summary {
  say_category "AUDIO GLUE"
  say dac~ "audio output"
  say adc~ "audio input"
  say sig~ "convert numbers to audio signals"
  say line~ "generate audio ramps"
  say vline~ "deluxe line~"
  say threshold~  "detect signal thresholds"
  say snapshot~ "sample a signal (convert it back to a number)"
  say vsnapshot~ "deluxe snapshot~"
  say bang~ "send a bang message after each DSP block"
  say samplerate~ "get the sample rate"
  say send~ "nonlocal signal connection with fanout"
  say receive~ "get signal from send~"
  say throw~ "add to a summing bus"
  say catch~ "define and read a summing bus"
  say block~ "specify block size and overlap"
  say switch~ "switch DSP computation on and off"
  say readsf~ "soundfile playback from disk"
  say writesf~ "record sound to disk"

  say_category "AUDIO OSCILLATORS AND TABLES"
  say phasor~ "sawtooth oscillator"
  say {cos~ osc~} "cosine oscillator"
  say tabwrite~ "write to a table"
  say tabplay~ "play back from a table (non-transposing)"
  say tabread~ "non-interpolating table read"
  say tabread4~ "four-point interpolating table read"
  say tabosc4~ "wavetable oscillator"
  say tabsend~ "write one block continuously to a table"
  say tabreceive~ "read one block continuously from a table"

  say_category "AUDIO FILTERS"
  say vcf~ "voltage controlled filter"
  say noise~ "white noise generator"
  say env~ "envelope follower"
  say hip~ "high pass filter"
  say lop~ "low pass filter"
  say bp~ "band pass filter"
  say biquad~ "raw filter"
  say samphold~ "sample and hold unit"
  say print~ "print out one or more \"blocks\""
  say rpole~ "raw real-valued one-pole filter"
  say rzero~ "raw real-valued one-zero filter"
  say rzero_rev~ "[say rzero~] (time-reversed)"
  say cpole~ "[say rpole~] (complex-valued)"
  say czero~ "[say rzero~] (complex-valued)"
  say czero_rev~ "[say rzero_rev~] (complex-valued)"

  say_category "AUDIO DELAY"
  say delwrite~ "write to a delay line"
  say delread~ "read from a delay line"
  say vd~ "read from a delay line at a variable delay time"

  say_category "SUBWINDOWS"
  say pd "define a subwindow"
  say table "array of numbers in a subwindow"
  say inlet "add an inlet to a pd"
  say outlet "add an outlet to a pd"
  say  inlet~  "[say inlet] (for signal)"
  say outlet~ "[say outlet] (for signal)"

  say_category "DATA TEMPLATES"
  say struct "define a data structure"
  say {drawcurve filledcurve} "draw a curve"
  say {drawpolygon filledpolygon} "draw a polygon"
  say plot "plot an array field"
  say drawnumber "print a numeric value"

  say_category "ACCESSING DATA"
  say pointer "point to an object belonging to a template"
  say get "get numeric fields"
  say set "change numeric fields"
  say element "get an array element"
  say getsize "get the size of an array"
  say setsize "change the size of an array"
  say append "add an element to a list"
  say sublist "get a pointer into a list which is an element of another scalar"
  say scalar "draw a scalar on parent"
  
  say_category "OBSOLETE"
  say scope~ "(use tabwrite~ now)"
  say namecanvas "" ;# what was this anyway?
  say template "(use struct now)"
}

# phase 4 (pdrc & ddrc)

say section_audio "Audio"
  say -r "sample rate"
  say -audioindev "audio in devices"
  say -audiooutdev "audio out devices"
  say -inchannels "audio input channels (by device, like \"2\" or \"16,8\")"
  say -outchannels "number of audio out channels (same)"
  say -audiobuf "specify size of audio buffer in msec"
  say -blocksize "specify audio I/O block size in sample frames"
  say -sleepgrain "specify number of milliseconds to sleep when idle"
  say -nodac "suppress audio output"
  say -noadc "suppress audio input"
  say audio_api_choice "Audio API"
    say default "default"
    say -alsa "use ALSA audio API"
    say -jack "use JACK audio API"
    say -mmio "use MMIO audio API (default for Windows)"
    say -portaudio "use ASIO audio driver (via Portaudio)"
    say -oss "use OSS audio API"
  say -32bit "allow 32 bit OSS audio (for RME Hammerfall)"
  say {} "default"

say section_midi "MIDI"
  say -nomidiin "suppress MIDI input"
  say -nomidiout "suppress MIDI output"
  say -midiindev  "midi in device list; e.g., \"1,3\" for first and third"
  say -midioutdev "midi out device list, same format"

say section_externals "Externals"
  say -path     "file search path"
  say -helppath "help file search path"
  say -lib      "load object libraries"

say section_gui "Gooey"
  say -nogui "suppress starting the GUI (caution)"
  say -guicmd "substitute another GUI program (e.g., rsh)"
  say -look "buttonbar icons"
  say -font "specify default font size in points"

say section_other "Other"
  say -open "open file(s) on startup"
  say -verbose "extra printout on startup and when searching for files"
  say -d "debug level"
  say -noloadbang "disable the effect of \[loadbang\]"
  say -send "send a message at startup (after patches are loaded)"
  say -listdev "list audio and MIDI devices upon startup"
  say -realtime "use real-time priority (needs root privilege)"

say section_paths "Paths"

# phase 4B: ddrc (keyword names not finalized!)
say console "console scrollback lines (0 = disable console)"
say lang "Language to use"
say pointer_sense "Mouse pointer sensitivity"
say section_color "colors"
 say canvas_color "canvas"
  say canvasbgedit "canvas background (edit mode)"
  say canvasbgrun "canvas background (run mode)"
 say object_color "object"
  say viewframe1 "objectbox color"
  say viewframe2 "objectbox color"
  say viewframe3 "objectbox color"
  say viewframe4 "objectbox highlight color"
  say viewbg "object background"
  say viewfg "object foreground"
  say commentbg "comment background"
  say commentfg "comment forground"
  say commentframe1 "comment frame"
  say commentframe2 "comment frame"
  say commentframe3 "comment frame"
  say viewselectframe "hilight box"
 say wire_color "wire"
  say wirefg "wire color"
  say wirefg2 "wire highlight"
  say wiredspfg "dsp wire color"
  say futurewiredash "new (dashed) wire"
 say others_color "others"
  say boxinletfg "inlet color"
  say boxoutletfg "outlet color"
  say selrectrect "selection box"
say keys "keys"
say others "others"
say hairstate "Activate crosshair"
say hairsnap "Crosshair snap to object"
say statusbar "Activate statusbar"
say buttonbar "Activate buttonbar"
say menubar "Activate menubar"
say scrollbar "Active auto scrollbar"
say wirearrow "Wire Arrow"
say tooltip "ToolTip"
say insert_object "Insert object"
say chain_object "Chain object"
say clear_wires "Clear wires"
say auto_wire "Remove object"
say subpatcherize "Subpatcherize"
say keynav "keyboard navigation"
say key_nav_up "move up"
say key_nav_up_shift "plus select"
say key_nav_down "move down"
say key_nav_down_shift "plus select"
say key_nav_right "move right"
say key_nav_right_shift "plus select"
say key_nav_left "move left"
say key_nav_left_shift "plus select"
say key_nav_ioselect "select in/outlets"
# phase 5A

say cannot "can't"
say cancel "Cancel"
say apply  "Apply"
say ok     "OK"
say popup_open "Open"
say popup_insert "Insert"
say popup_properties "Properties"
say popup_clear_wires "Clear wires"
say popup_remove_from_path "Remove object from path"
say popup_delete_from_path "Delete object from path"
say popup_help "Help"
say filter "Filter: "
say how_many_object_classes "%d of %d object classes"
say do_what_i_mean "Do What I Mean"
say ask_cool "This would be a cool feature, eh?"
say save_changes? "Save changes?"
say reset "Reset"
say Courier "Courier (monospaced)"
say Helvetica "Helvetica (sansserif)"
say Times "Times (serif)"
say add "Add"
say up "Up"
say down "Down"
say remove "Remove"
say lib_add    "add the name you typed to the list"
say lib_up     "swap order with previous library"
say lib_down   "swap order with next library"
say lib_remove "remove library selected in the list"
say dir_add    "add a folder using a file dialog"
say dir_up     "swap order with previous folder"
say dir_down   "swap order with next folder"
say dir_remove "remove folder selected in the list"
say client_class_tree "Client Class Tree"
say clipboard_view "Clipboard View"
say command_history_view "Command History View"
say event_history_view "Event History View"

# during/after piksel:

say auto_apply "Auto-Apply"
say font_preview "Preview:"
say font_preview_2 "ABCDEFGHIJKLMNOPQRSTUVWXYZ\nabcdefghijklmnopqrstuvwxyz\n0123456789"
say font_style "Style:"
say font_bold "Bold"
say font_italic "Italic"
say font_family "Name:"
say font_size "Size:"
say damn "Damn!"
say console_clear "Clear Console"
say horizontal "Horizontal"
say vertical "Vertical"
say language "Language"

# 2007:

say no_matches "(no matches)"
say preset "preset"

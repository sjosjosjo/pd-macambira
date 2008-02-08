#!/usr/bin/env tclsh
# $Id: dansk.tcl,v 1.1.2.2 2007-08-18 18:01:10 matju Exp $
# Danish translations for PureData
# by Steffen Leve Poulsen

### Menus

say file "Fil"
  say new_file "Ny fil"
  say open_file "�bn fil..."
  say server_prefs "Server Preferenser..."
  say client_prefs "Klient Preferenser..."
  say send_message "Send besked..."
  say paths "Stier..."
  say close "Luk"
  say save "Gem"
  say save_as "Gem som..."
  say print "Udskriv..."
  say abort_server "Stop server"
  say quit "Afslut"
  
  say canvasnew_file "Ny Fil"
  say canvasopen_file "�bn Fil..."
  say canvassave "Gem"
  say canvassave_as "Gem som..."
  say clientpdrc_editor ".pdrc reidgering"
  say clientddrc_editor ".ddrc redigering"
  say canvasclose "Luk"
  say canvasquit "Afslut"

say edit "Rediger"
  say undo "Fortryd"
  say redo "Genskab"
  say cut "Klip"
  say copy "Kopier"
  say paste "S�t ind"
  say duplicate "Dubler"
  say select_all "V�lg alt"
  say clear_selection "Afv�lg"
  say text_editor "Tekstredigering..."
  say font "Font"
  say tidy_up "Ordn"
  say edit_mode "Rediger"
  say editmodeswitch "Rediger/K�r tilstand"
  say subpatcherize "G�r til underlap"

  say canvascut "Klip"
  say canvascopy "Kopier"
  say canvasundo "Fortryd"
  say canvasredo "Genskab"
  say canvaspaste "S�t ind"
  say canvasduplicate "Dubler"
  say canvasselect_all "V�lg alt"
  say canvaseditmodeswitch "Rediger/K�r tilstand"

say view "Se"
  say reload "�bn igen"
  say redraw "Gentegn"
  
  say canvasreload "�bn igen"
  say canvasredraw "Gentegn"

say find "S�g"
  say find_again "S�g igen"
  say find_last_error "Find sidste fejl"
  say string "Find streng"
say canvasfind "S�g"
  say canvasfind_again "S�g igen"

# contents of Put menu is Phase 5C
say put "Inds�t"
  say Object "Objekt"
  say Message "Besked"
  say Number "Tal"
  say Symbol "Symbol"
  say Comment "Kommentar"
  say Graph "Graf"
  say Array "Tabel"

say media "Medier"
  say audio_on "Start lyd"
  say audio_off "Stop lyd"
  say test_audio_and_midi "Test lyd og MIDI"
  say load_meter "Belastning"

  say canvasaudio_on "Start lyd"
  say canvasaudio_off "Stop lyd"
  say clienttest_audio_and_midi "Test lyd og MIDI"
  say canvasload_meter "Belastning"

say window "Vindue"

say help "Hj�lp"
  say about "Om..."
  say documentation "Dokumentation..."
  say class_browser "Se klasser..."
  
  say canvasabout "Om..."

say properties "Egenskaber"
say open "�bn"

### for key binding editor
say general "Generelt"
say audio_settings "Lyd indstillinger"
say midi_settings "MIDI indstillinger"
say latency_meter "Forsinkelse"
say Pdwindow "Pd vindue"

say canvaspdwindow "Pd vindue"
say canvaslatency_meter "Forsinkelse"
say clientaudio_settings "Lyd indstillinger"
say clientmidi_settings "MIDI indstillinger"

### for Properties Dialog (phase 5B)
say_category IEM
say w "bredde(px)"
say h "h�jde(px)"
say hold "vis (ms)"
say break "afbryd (ms)"
say min "minimum v�rdi"
say max "maximum v�rdi"
say is_log "modus"
say linear "line�r"
say logarithmic "logaritmisk"
say isa "startv�rdi"
say n "antal valg"
say steady "reaktion"
say steady_no  "Hop ved klik"
say steady_yes "Bliv ved klik"
say snd "sende-navn"
say rcv "modtage-navn"
say lab "etiket"
say ldx "etiket x afs�t"
say ldy "etiket y afs�t"
say fstyle "Udseende"
say fs "font st�rrelse"
say bcol "baggrundsfarve"
say fcol "forgrundsfarve"
say lcol "farve etiket"
say yes "ja"
say no "nej"
say courier "courier (typewriter)"
say helvetica "helvetica (sansserif)"
say times "times (serif)"
say coords "vis p� for�ldre"

say_category GAtomProperties
say width "bredde"
say lo "nedre gr�nse"
say hi "�vre gr�nse"
say label "etiket"
say wherelabel "vis label"
say symto "sende-navn"
say symfrom "modtage-navn"

say_category GraphProperties
say x1   "x fra"
say x2   "x til"
say xpix "bredde"
say y2   "y fra"
say y1   "y til"
say ypix "h�jde"

say_category CanvasProperties
#say xscale "X enheder/px"
#say yscale "Y enheder/px"
say gop "vis p� for�lder"
say xmargin "x margen"
say ymargin "y margen"
say height "h�jde"
say_category ArrayProperties
say name "navn"
say n    "st�rrelse"
say xfrom "x fra"
say xto "x til"
say yfrom "y fra"
say yto "y til"


say_category MainWindow
say in "ind"
say out "ud"
say audio "Lyd"
say meters "Meter"
say io_errors "IO fejl"
say tcl_console "Tcl Klient"
say pd_console "Pd Server"

### phase 2

say_category Other
say_namespace summary {
  say_category IEMGUI
  say bng    "Bang Boks"
  say tgl    "Toggle Boks"
  say nbx    "Tal Boks (IEM)"
  say hsl    "Fader (vandret)"
  say vsl    "Fader (lodret)"
  say hradio "Valgboks (vandret)"
  say vradio "Valgbox (lodret)"
  say cnv    "L�rred (IEM)"
  say dropper "Tr�k og slip box"
  say vu     "Vumeter"

  say_category GLUE
  say bang "send et bang"
  say float "gem og genkald decimaltal"
  say symbol "gem og genkald symbol"
  say int "gem og genkald heltal"
  say send "send besked til objekt med navn"
  say receive "modtag sendte beskeder"
  say select "v�lg symbol eller tal"
  say route "rute efter f�rste element"
  say pack "pak elementer"
  say unpack "pak elementer ud"
  say trigger "r�kkef�lge og omdannelse"
  say spigot "afbryder"
  say moses "del talstr�m"
  say until "gentagne bangs"
  say print "skriv til promt"
  say makefilename "formater navn med variabel"
  say change "fjern gentagelser"
  say swap "ombyt to tal"
  say value "global variabel"

  say_category TIME
  say delay "forsink bang"
  say metro "metronom"
  say line "skridtvis line�r interpolation"
  say timer "m�l tid mellem beskeder"
  say cputime "m�l CPU tid"
  say realtime "m�l reel tid"
  say pipe "dynamisk voksende forsinkelse af tal"

  say_category MATH
  say + "adder"
  say - "subtraher"
  say * "multiplicer"
  say {/ div} "divider"
  say {% mod} "heltal modulus"
  say pow "eksponent"
  say == "ens?"
  say != "forskellig?"
  say > "st�rrer end?"
  say < "mindre end?"
  say >= "st�rrer eller lig med?"
  say <= "mindre eller lig med?"
  say &  "bitm�ssig (og)"
  say |  "bitm�ssig (eller)"
  say && "boelsk (og)"
  say || "boelsk (eller)"
  say mtof "MIDI til Hertz"
  say ftom "Hertz til MIDI"
  say powtodb "W til dB"
  say dbtopow "dB til W"
  say rmstodb "Volt til dB"
  say dbtorms "dB til Volt"
  say {sin cos tan atan atan2 sqrt} "trigonometri"
  say log "Euler logaritme"
  say exp "Euler exponentiale"
  say abs "absolutte v�rdi"
  say random "tilf�ldigt heltal"
  say max "st�rst af to tal"
  say min "mindst af to tal"
  say clip "klip tal hvis udenfor gr�nser"

  say_category MIDI
  say {notein ctlin pgmin bendin touchin polytouchin midiin sysexin} "MIDI ind"
  say {noteout ctlout pgmout bendout touchout polytouchout midiout}  "MIDI ud"
  say makenote "formater miditone"
  say stripnote "fjern \"note off\" beskeder"

  say_category TABLES
  say tabread "l�s tal fra tabel"
  say tabread4 "l�s tal fra tabel, med kubisk interpolation"
  say tabwrite "skriv tal ind i tabel"
  say soundfiler "l�s og skriv tabeller"

  say_category MISC
  say loadbang "bang ved start"
  say serial "serielport (kun NT)"
  say netsend "send bekeder via netv�rk"
  say netreceive "modtag beskeder fra netv�rk"
  say qlist "besked sequencer"
  say textfile "fil til besked konverter"
  say openpanel "\"�bn\" dialog"
  say savepanel "\"Gem som\" dialog"
  say bag "tals�t"
  say poly "polyfonisk stemmekontrol"
  say {key keyup} "tastaturv�rdier"
  say keyname "tastnavn"

  say_category "Signal matematik"
  foreach word {+ - * /} {say $word~ "[say $word] (for signals)"}
  say max~ "st�rst af to signaler"
  say min~  "mindst af to signaler"
  say clip~ "begr�ns signal"
  say q8_rsqrt~ "billig omvendt kvadratrod (8 bit!)"
  say q8_sqrt~ "billig kvadratrod (8 bit!)"
  say wrap~ "decimaldel af signal"
  say fft~ "compleks diskret Fourier transformation"
  say ifft~ "omvendt compleks diskret Fourier transformation"
  say rfft~ "reel diskret Fourier transformation"
  say rifft~ "omvendt reel diskret Fourier transformation"
  say framp~ "rampe per blok"
  foreach word {mtof ftom rmstodb dbtorms rmstopow powtorms} {
    say $word~ "[say $word] (for signals)"
  }
}

### phase 3

say_namespace summary {
  say_category "LYD LIM"
  say dac~ "lyd ud"
  say adc~ "lyd ind"
  say sig~ "konverter tal til signal"
  say line~ "signal rampe"
  say vline~ "deluxe line~"
  say threshold~  "find signalgr�nse"
  say snapshot~ "konverter signal til tal"
  say vsnapshot~ "deluxe snapshot~"
  say bang~ "bang efter hver blok"
  say samplerate~ "hent samplefrekvens"
  say send~ "send et signal til mange modtagere"
  say receive~ "modtag fra send~"
  say throw~ "mange signalafsendere til en modtager"
  say catch~ "modtag fra throw~"
  say block~ "s�t blokst�rrelse og overlap"
  say switch~ "t�nd/sluk DSP lokalt"
  say readsf~ "afspil lydfil fra h�rdskive"
  say writesf~ "optag lyd p� h�rdskive"

  say_category "signal oscillatorer og tabeller"
  say phasor~ "savtak-generator"
  say {cos~ osc~} "cubisk interpoleret cosinus tabelopslag"
  say tabwrite~ "skriv signal til tabel"
  say tabplay~ "afspil tabel (uden transponering)"
  say tabread~ "ikke interpoleret tabelafspilning"
  say tabread4~ "kubisk interpoleret tabelafspilning"
  say tabosc4~ "tabel oscillator"
  say tabsend~ "skriv en blok kontinuerligt til tabel"
  say tabreceive~ "l�s en blok kontinuerligt fra tabel"

  say_category "Signal filtre"
  say vcf~ "volt controlleret filter"
  say noise~ "hvid st�j"
  say env~ "f�lg signal"
  say hip~ "h�jpas filter"
  say lop~ "lavpas filter"
  say bp~ "b�ndpas filter"
  say biquad~ "2-pol-2-nul-filter"
  say samphold~ "sample og frys dims"
  say print~ "skriv en eller flere \"blocks\" ud"
  say rpole~ "en pol reel filter"
  say rzero~ "et nul reel filter"
  say rzero_rev~ "[say rzero~] (bagl�ns)"
  say cpole~ "[say rpole~] (kompleks)"
  say czero~ "[say rzero~] (kompleks)"
  say czero_rev~ "[say rzero_rev~] (kompleks)"

  say_category "AUDIO DELAY"
  say delwrite~ "forsink"
  say delread~ "l�s efter forsinkelse"
  say vd~ "l�s efter varieret forsinkelse"

  say_category "SUBWINDOWS"
  say pd "definer undervindue"
  say table "tabel i undervindue"
  say inlet "tilf�j indgang til et vindue"
  say outlet "tilf�j udgang fra et vindue"
  say  inlet~  "[say inlet] (signal)"
  say outlet~ "[say outlet] (signal)"

  say_category "DATA skabeloner"
  say struct "definer en data struktur"
  say {drawcurve filledcurve} "tegn en kurve"
  say {drawpolygon filledpolygon} "tegn en polygon"
  say plot "tegn en tabel"
  say drawnumber "skriv et tal"

  say_category "ACCESSING DATA"
  say pointer "peg p� et objekt der h�rer til en skabelon"
  say get "hent numeriske felt"
  say set "skift numerisk felt"
  say element "hent element fra tabel"
  say getsize "hent st�rrelse p� tabel"
  say setsize "s�t l�ngde p� tabel"
  say append "tilf�j element til liste"
  say sublist "peger p� start af undertabel"
  say scalar "tegn skalar p� for�lder"
  
  say_category "FOOR�LDET"
  say scope~ "(brug tabwrite~ nu)"
  say namecanvas "" ;# what was this anyway?
  say template "(brug struct)"
}

# phase 4 (pdrc & ddrc)

say section_audio "Lyd"
  say -r "sample rate"
  say -audioindev "LYD-ind enheder"
  say -audiooutdev "LYD-ud enheder"
  say -inchannels "antal lydkanaler ind (per enhed, som \"2\" or \"16,8\")"
  say -outchannels "antal lydkanaler ud (samme)"
  say -audiobuf "Lydbuffer i millisekunder"
  say -blocksize "specificer lyd I/O blok i samples"
  say -sleepgrain "specificer pause i millisekunder ved ledighed"
  say -nodac "ingen lyd ud"
  say -noadc "ingen lyd ind"
  say audio_api_choice "Audio API"
    say default "forudindstillet"
    say -alsa "brug ALSA lyd API"
    say -jack "brug JACK lyd API"
    say -mmio "brug MMIO lyd API (forudindstillet til Windows)"
    say -portaudio "brug ASIO lyd (via Portaudio)"
    say -oss "brug OSS lyd API"
  say -32bit "tillad 32 bit OSS lyd (til RME Hammerfall)"
  say {} "forudindstillet"

say section_midi "MIDI"
  say -nomidiin "ingen MIDI ind"
  say -nomidiout "ingen MIDI ud"
  say -midiindev  "midi ind enhedsliste; e.g., \"1,3\" f�rste og tredje"
  say -midioutdev "midi ud enhedsliste (samme format)"

say section_externals "Eksterne biblioteker"
  say -path     "fil s�gesti"
  say -helppath "hj�lpefil s�gesti"
  say -lib      "�bn biblioteker"

say section_gui "Gooey"
  say -nogui "ingen grafisk brugerflade (pas p�)"
  say -guicmd "brug anden grafisk brugerflade (e.g., rsh)"
  say -look "knap ikoner"
  say -font "s�t forudindstillet fontst�rrelse i points"

say section_other "Andet"
  say -open "�bn fil(er) ved start"
  say -verbose "ekstra beskeder ved opstart og fils�gning"
  say -d "fejls�gning"
  say -noloadbang "ingen \[loadbang\]"
  say -send "send besked ved start (efter alt er �bent)"
  say -listdev "list lyd og MIDI enheder ved start"
  say -realtime "brug real-time priority (beh�ver root privilege)"

say section_paths "Stier"

# phase 4B: ddrc (keyword names not finalized!)
say console "spol antal linier tilbage i konsollen (0 = skriv ikke til konsol)"
say lang "Sprog"
say pointer_sense "Musens f�lsomhed"
say section_color "farver"
 say canvas_color "l�rred"
  say canvasbgedit "farve p� l�rred ved redigering"
  say canvasbgrun "farve p� l�rred ved K�r"
 say object_color "objekt"
  say viewframe1 "farve p� objektboks"
  say viewframe2 "farve p� objektboks"
  say viewframe3 "farve p� objektboks"
  say viewframe4 "farve p� valgt objektboks"
  say viewbg "baggrundsfarve p� objektboks"
  say viewfg "forgrundsfarve p� objektboks"
  say commentbg "baggrundsfarve p� kommentar"
  say commentfg "forgrundsfarve p� kommentar"
  say commentframe1 "ramme p� kommentar"
  say commentframe2 "ramme p� kommentar"
  say commentframe3 "ramme p� kommentar"
  say viewselectframe "valgt boks"
 say wire_color "tr�d"
  say wirefg "tr�d farve"
  say wirefg2 "valgt tr�d"
  say wiredspfg "signaltr�d"
  say futurewiredash "ny (stiplet) tr�d"
 say others_color "andet"
  say boxinletfg "farve p� indgang"
  say boxoutletfg "farve p� udgang"
  say selrectrect "valgt omr�de"
say keys "taster"
say others "andet"
say hairstate "aktiver kors"
say hairsnap "snap kors til objekt"
say statusbar "Aktiver statusvisning"
say buttonbar "Aktiver objekt knapper"
say menubar "Aktiver menubar"
say scrollbar "Aktiver autospoleknap"
say wirearrow "tr�d pil"
say tooltip "V�rkt�jstip"
say insert_object "Inds�t objekt"
say chain_object "Forbind objekt(er)"
say clear_wires "Fjern tr�de"
say auto_wire "Fjern objekt"
say subpatcherize "Subpatcherize"
say keynav "tastatur navigation"
say key_nav_up "flyt op"
say key_nav_up_shift "plus select"
say key_nav_down "flyt ned"
say key_nav_down_shift "plus select"
say key_nav_right "flyt til h�jre"
say key_nav_right_shift "plus select"
say key_nav_left "flyt til venstre"
say key_nav_left_shift "plus select"
say key_nav_ioselect "v�lg ind/udgang"
# phase 5A

say cannot "Kan ikke"
say cancel "fortryd"
say apply  "G�r det"
say ok     "OK"
say popup_open "�bn"
say popup_insert "S�t ind"
say popup_properties "Egenskaber"
say popup_clear_wires "Slet tr�de"
say popup_remove_from_path "Fjern objekt fra sti"
say popup_delete_from_path "Slet objekt fra sti"
say popup_help "Hj�lp"
say filter "Filter: "
say how_many_object_classes "%d af %d objektklasser"
say do_what_i_mean "G�r hvad jeg mener"
say ask_cool "Det ville have v�ret fedt at kunne g�re, ikk?"
say save_changes? "Gem �ndringer?"
say reset "Nulstil"
say Courier "Courier (monospaced)"
say Helvetica "Helvetica (sansserif)"
say Times "Times (serif)"
say add "Tilf�j"
say up "Op"
say down "Ned"
say remove "Fjern"
say lib_add    "tilf�j bibliotek til listen"
say lib_up     "byt med forrige bibliotek"
say lib_down   "byt med n�ste bibliotek"
say lib_remove "fjern valgte fra listen"
say dir_add    "tilf�j mappe vha. dialog"
say dir_up     "byt med forrige mappe"
say dir_down   "byt med n�ste mappe"
say dir_remove "fjern valgt mappe fra listen"
say client_class_tree "Klient klasse tr�"
say clipboard_view "Se klipbord"
say command_history_view "se kommando historie"
say event_history_view "se handlings historie"

# during/after piksel:

say auto_apply "Automatisk"
say font_preview "Se:"
say font_preview_2 "ABCDEFGHIJKLMNOPQRSTUVWXYZ\nabcdefghijklmnopqrstuvwxyz\n0123456789"
say font_style "Stil:"
say font_bold "Fremh�vet"
say font_italic "Skr�"
say font_family "Navn:"
say font_size "St�rrelse:"
say damn "Pokkers!"
say console_clear "Slet indholdet i konsollen"
say horizontal "Vandret"
say vertical "Lodret"
say language "Sprog"

# 2007:

say no_matches "(ingen svarer til)"
say preset "forudsat"

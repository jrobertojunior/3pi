﻿#include <Simulator.h>
#include <Pololu3pi.h>
#include <OrangutanMotors.h>
#include <OrangutanPushbuttons.h>
#include <OrangutanLCD.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <thread>

#include <sstream>

#include <timer.h>

#include <cvui.h>

int Pololu3pi::sensors[5];
int OrangutanPushbuttons::buttonAState;
int OrangutanPushbuttons::buttonBState;
int OrangutanPushbuttons::buttonCState;
float OrangutanMotors::vright;
float OrangutanMotors::vleft;

int OrangutanLCD::x;
int OrangutanLCD::y;
int OrangutanLCD::offsetx;
int OrangutanLCD::offsety;
char OrangutanLCD::content[2][8];


char OrangutanLCD::ascii[256][8] = {
	// implementacao do repo 
	// github.com/idispatch/raster-fonts/blob/master/font-5x8.c
	
	// code=0, hex=0x00, ascii="^@"
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	// code=1, hex=0x01, ascii="^A"
	{ 0x00, 0x00, 0x70, 0xA8, 0xF8, 0xD8, 0x70, 0x00 },
	// code=2, hscii="^B"
	{ 0x00, 0x00, 0x70, 0xA8, 0xF8, 0xF8, 0x70, 0x00 },
	// code=3, hscii="^C"
	{ 0x00, 0x00, 0x50, 0xF8, 0xF8, 0x70, 0x20, 0x00 },
	// code=4, hscii="^D"
	{ 0x00, 0x00, 0x20, 0x70, 0xF8, 0x70, 0x20, 0x00 },
	// code=5, hscii="^E"
	{ 0x00, 0x00, 0x70, 0xA8, 0xF8, 0x20, 0x20, 0x00 },
	// code=6, hscii="^F"
	{ 0x00, 0x00, 0x20, 0x70, 0xF8, 0xA8, 0x20, 0x00 },
	// code=7, hscii="^G"
	{ 0x00, 0x00, 0x00, 0x20, 0x70, 0x20, 0x00, 0x00 },
	// code=8, hscii="^H"
	{ 0x00, 0xF8, 0xF8, 0xD8, 0x88, 0xD8, 0xF8, 0xF8 },
	// code=9, hscii="^I"
	{ 0x00, 0x00, 0x00, 0x20, 0x50, 0x20, 0x00, 0x00 },
	// code=10, ascii="^J"
	{ 0x00, 0xF8, 0xF8, 0xD8, 0x88, 0xD8, 0xF8, 0xF8 },
	// code=11, ascii="^K"
	{ 0x00, 0x00, 0x38, 0x18, 0x68, 0xA0, 0x40, 0x00 },
	// code=12, ascii="^L"
	{ 0x00, 0x00, 0x20, 0x50, 0x20, 0x70, 0x20, 0x00 },
	// code=13, ascii="^M"
	{ 0x00, 0x00, 0x20, 0x50, 0x40, 0xC0, 0x80, 0x00 },
	// code=14, ascii="^N"
	{ 0x00, 0x00, 0x38, 0x48, 0x58, 0xD0, 0x80, 0x00 },
	// code=15, ascii="^O"
	{ 0x00, 0x00, 0x00, 0x20, 0x50, 0x20, 0x00, 0x00 },
	// code=16, ascii="^P"
	{ 0x00, 0x00, 0x40, 0x60, 0x70, 0x60, 0x40, 0x00 },
	// code=17, ascii="^Q"
	{ 0x00, 0x00, 0x10, 0x30, 0x70, 0x30, 0x10, 0x00 },
	// code=18, ascii="^R"
	{ 0x00, 0x00, 0x20, 0x70, 0x20, 0x70, 0x20, 0x00 },
	// code=19, ascii="^S"
	{ 0x00, 0x00, 0x50, 0x50, 0x50, 0x00, 0x50, 0x00 },
	// code=20, ascii="^T"
	{ 0x00, 0x00, 0x78, 0xD0, 0xD0, 0x50, 0x50, 0x50 },
	// code=21, ascii="^U"
	{ 0x00, 0x00, 0x18, 0x60, 0x90, 0x48, 0x30, 0xC0 },
	// code=22, ascii="^V"
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0xF8, 0x00 },
	// code=23, ascii="^W"
	{ 0x00, 0x00, 0x20, 0x70, 0x20, 0x70, 0x20, 0x70 },
	// code=24, ascii="^X"
	{ 0x00, 0x00, 0x20, 0x70, 0x20, 0x20, 0x20, 0x00 },
	// code=25, ascii="^Y"
	{ 0x00, 0x00, 0x20, 0x20, 0x20, 0x70, 0x20, 0x00 },
	// code=26, ascii="^Z"
	{ 0x00, 0x00, 0x00, 0x10, 0xF8, 0x10, 0x00, 0x00 },
	// code=27, ascii="^["
	{ 0x00, 0x00, 0x00, 0x40, 0xF8, 0x40, 0x00, 0x00 },
	// code=28, ascii="^\"
	{ 0x00, 0x00, 0x00, 0x00, 0x80, 0xF8, 0x00, 0x00 },
	// code=29, ascii="^]"
	{ 0x00, 0x00, 0x00, 0x50, 0xF8, 0x50, 0x00, 0x00 },
	// code=30, ascii="^^"
	{ 0x00, 0x00, 0x00, 0x00, 0x20, 0x70, 0xF8, 0x00 },
	// code=31, ascii="^_"
	{ 0x00, 0x00, 0x00, 0x00, 0xF8, 0x70, 0x20, 0x00 },
	// code=32, ascii=" "
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	// code=33, ascii="!"
	{ 0x00, 0x00, 0x20, 0x20, 0x20, 0x00, 0x20, 0x00 },
	// code=34, ascii="""
	{ 0x00, 0x00, 0x50, 0x50, 0x00, 0x00, 0x00, 0x00 },
	// code=35, ascii="#"
	{ 0x00, 0x00, 0x50, 0xF8, 0x50, 0xF8, 0x50, 0x00 },
	// code=36, ascii="$"
	{ 0x00, 0x00, 0x20, 0x30, 0x40, 0x30, 0x60, 0x20 },
	// code=37, ascii="%"
	{ 0x00, 0x40, 0xA8, 0x50, 0x30, 0x68, 0x90, 0x00 },
	// code=38, ascii="&"
	{ 0x00, 0x00, 0x30, 0x40, 0x68, 0x90, 0x68, 0x00 },
	// code=39, ascii="'"
	{ 0x00, 0x20, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00 },
	// code=40, ascii="("
	{ 0x00, 0x00, 0x20, 0x40, 0x40, 0x40, 0x20, 0x00 },
	// code=41, ascii=")"
	{ 0x00, 0x00, 0x40, 0x20, 0x20, 0x20, 0x40, 0x00 },
	// code=42, ascii="*"
	{ 0x00, 0x00, 0x50, 0x20, 0x70, 0x20, 0x50, 0x00 },
	// code=43, ascii="+"
	{ 0x00, 0x00, 0x00, 0x20, 0x70, 0x20, 0x00, 0x00 },
	// code=44, ascii=","
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x40 },
	// code=45, ascii="-"
	{ 0x00, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00 },
	// code=46, ascii="."
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00 },
	// code=47, ascii="/"
	{ 0x00, 0x00, 0x10, 0x20, 0x20, 0x40, 0x40, 0x00 },
	// code=48, ascii="0"
	{ 0x00, 0x00, 0x60, 0x90, 0x90, 0x90, 0x60, 0x00 },
	// code=49, ascii="1"
	{ 0x00, 0x00, 0x20, 0x60, 0x20, 0x20, 0x20, 0x00 },
	// code=50, ascii="2"
	{ 0x00, 0x00, 0x60, 0x90, 0x20, 0x40, 0xF0, 0x00 },
	// code=51, ascii="3"
	{ 0x00, 0x00, 0xE0, 0x10, 0x60, 0x10, 0xE0, 0x00 },
	// code=52, ascii="4"
	{ 0x00, 0x00, 0x10, 0x30, 0x50, 0xF0, 0x10, 0x00 },
	// code=53, ascii="5"
	{ 0x00, 0x00, 0xF0, 0x80, 0xE0, 0x10, 0xE0, 0x00 },
	// code=54, ascii="6"
	{ 0x00, 0x00, 0x60, 0x80, 0xE0, 0x90, 0x60, 0x00 },
	// code=55, ascii="7"
	{ 0x00, 0x00, 0xF0, 0x10, 0x20, 0x40, 0x40, 0x00 },
	// code=56, ascii="8"
	{ 0x00, 0x00, 0x60, 0x90, 0x60, 0x90, 0x60, 0x00 },
	// code=57, ascii="9"
	{ 0x00, 0x00, 0x60, 0x90, 0x70, 0x10, 0x60, 0x00 },
	// code=58, ascii=":"
	{ 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x20, 0x00 },
	// code=59, ascii=";"
	{ 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x20, 0x40 },
	// code=60, ascii="<"
	{ 0x00, 0x00, 0x10, 0x20, 0x40, 0x20, 0x10, 0x00 },
	// code=61, ascii="="
	{ 0x00, 0x00, 0x00, 0x70, 0x00, 0x70, 0x00, 0x00 },
	// code=62, ascii=">"
	{ 0x00, 0x00, 0x40, 0x20, 0x10, 0x20, 0x40, 0x00 },
	// code=63, ascii="?"
	{ 0x00, 0x00, 0x60, 0x10, 0x60, 0x00, 0x40, 0x00 },
	// code=64, ascii="@"
	{ 0x00, 0x00, 0x70, 0x88, 0xB0, 0x80, 0x70, 0x00 },
	// code=65, ascii="A"
	{ 0x00, 0x00, 0x60, 0x90, 0xF0, 0x90, 0x90, 0x00 },
	// code=66, ascii="B"
	{ 0x00, 0x00, 0xE0, 0x90, 0xE0, 0x90, 0xE0, 0x00 },
	// code=67, ascii="C"
	{ 0x00, 0x00, 0x70, 0x80, 0x80, 0x80, 0x70, 0x00 },
	// code=68, ascii="D"
	{ 0x00, 0x00, 0xE0, 0x90, 0x90, 0x90, 0xE0, 0x00 },
	// code=69, ascii="E"
	{ 0x00, 0x00, 0xF0, 0x80, 0xE0, 0x80, 0xF0, 0x00 },
	// code=70, ascii="F"
	{ 0x00, 0x00, 0xF0, 0x80, 0xE0, 0x80, 0x80, 0x00 },
	// code=71, ascii="G"
	{ 0x00, 0x00, 0x60, 0x90, 0x80, 0x90, 0x70, 0x00 },
	// code=72, ascii="H"
	{ 0x00, 0x00, 0x90, 0x90, 0xF0, 0x90, 0x90, 0x00 },
	// code=73, ascii="I"
	{ 0x00, 0x00, 0x70, 0x20, 0x20, 0x20, 0x70, 0x00 },
	// code=74, ascii="J"
	{ 0x00, 0x00, 0x10, 0x10, 0x90, 0x90, 0x60, 0x00 },
	// code=75, ascii="K"
	{ 0x00, 0x00, 0x90, 0xA0, 0xC0, 0xA0, 0x90, 0x00 },
	// code=76, ascii="L"
	{ 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0xF0, 0x00 },
	// code=77, ascii="M"
	{ 0x00, 0x00, 0x90, 0x90, 0xF0, 0x90, 0x90, 0x00 },
	// code=78, ascii="N"
	{ 0x00, 0x00, 0x90, 0xD0, 0xB0, 0x90, 0x90, 0x00 },
	// code=79, ascii="O"
	{ 0x00, 0x00, 0x60, 0x90, 0x90, 0x90, 0x60, 0x00 },
	// code=80, ascii="P"
	{ 0x00, 0x00, 0xE0, 0x90, 0xE0, 0x80, 0x80, 0x00 },
	// code=81, ascii="Q"
	{ 0x00, 0x00, 0x60, 0x90, 0x90, 0x90, 0x60, 0x10 },
	// code=82, ascii="R"
	{ 0x00, 0x00, 0xE0, 0x90, 0xE0, 0x90, 0x90, 0x00 },
	// code=83, ascii="S"
	{ 0x00, 0x00, 0x70, 0x80, 0x60, 0x10, 0xE0, 0x00 },
	// code=84, ascii="T"
	{ 0x00, 0x00, 0xF8, 0x20, 0x20, 0x20, 0x20, 0x00 },
	// code=85, ascii="U"
	{ 0x00, 0x00, 0x90, 0x90, 0x90, 0x90, 0x60, 0x00 },
	// code=86, ascii="V"
	{ 0x00, 0x00, 0x90, 0x90, 0x90, 0x60, 0x60, 0x00 },
	// code=87, ascii="W"
	{ 0x00, 0x00, 0x88, 0xA8, 0xA8, 0x50, 0x50, 0x00 },
	// code=88, ascii="X"
	{ 0x00, 0x00, 0x90, 0x90, 0x60, 0x50, 0x90, 0x00 },
	// code=89, ascii="Y"
	{ 0x00, 0x00, 0x50, 0x50, 0x50, 0x20, 0x20, 0x00 },
	// code=90, ascii="Z"
	{ 0x00, 0x00, 0xF0, 0x20, 0x40, 0x80, 0xF0, 0x00 },
	// code=91, ascii="["
	{ 0x00, 0x00, 0x60, 0x40, 0x40, 0x40, 0x60, 0x00 },
	// code=92, ascii="\"
	{ 0x00, 0x00, 0x40, 0x40, 0x20, 0x20, 0x10, 0x00 },
	// code=93, ascii="]"
	{ 0x00, 0x00, 0x60, 0x20, 0x20, 0x20, 0x60, 0x00 },
	// code=94, ascii="^"
	{ 0x00, 0x00, 0x20, 0x50, 0x00, 0x00, 0x00, 0x00 },
	// code=95, ascii="_"
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8 },
	// code=96, ascii="`"
	{ 0x00, 0x00, 0x40, 0x20, 0x00, 0x00, 0x00, 0x00 },
	// code=97, ascii="a"
	{ 0x00, 0x00, 0x00, 0x60, 0x10, 0x70, 0x50, 0x00 },
	// code=98, ascii="b"
	{ 0x00, 0x80, 0x80, 0xE0, 0x90, 0x90, 0xE0, 0x00 },
	// code=99, ascii="c"
	{ 0x00, 0x00, 0x00, 0x30, 0x40, 0x40, 0x30, 0x00 },
	// code=100, ascii="d"
	{ 0x00, 0x00, 0x10, 0x70, 0x90, 0x90, 0x70, 0x00 },
	// code=101, ascii="e"
	{ 0x00, 0x00, 0x00, 0x60, 0xF0, 0x80, 0x70, 0x00 },
	// code=102, ascii="f"
	{ 0x00, 0x00, 0x30, 0x40, 0xE0, 0x40, 0x40, 0x00 },
	// code=103, ascii="g"
	{ 0x00, 0x00, 0x00, 0x70, 0x90, 0x70, 0x10, 0x60 },
	// code=104, ascii="h"
	{ 0x00, 0x00, 0x80, 0xE0, 0x90, 0x90, 0x90, 0x00 },
	// code=105, ascii="i"
	{ 0x00, 0x20, 0x00, 0x60, 0x20, 0x20, 0x70, 0x00 },
	// code=106, ascii="j"
	{ 0x00, 0x10, 0x00, 0x10, 0x10, 0x10, 0x10, 0x60 },
	// code=107, ascii="k"
	{ 0x00, 0x00, 0x80, 0xA0, 0xC0, 0xA0, 0x90, 0x00 },
	// code=108, ascii="l"
	{ 0x00, 0x60, 0x20, 0x20, 0x20, 0x20, 0x70, 0x00 },
	// code=109, ascii="m"
	{ 0x00, 0x00, 0x00, 0x90, 0xF0, 0x90, 0x90, 0x00 },
	// code=110, ascii="n"
	{ 0x00, 0x00, 0x00, 0xE0, 0x90, 0x90, 0x90, 0x00 },
	// code=111, ascii="o"
	{ 0x00, 0x00, 0x00, 0x60, 0x90, 0x90, 0x60, 0x00 },
	// code=112, ascii="p"
	{ 0x00, 0x00, 0x00, 0xE0, 0x90, 0x90, 0xE0, 0x80 },
	// code=113, ascii="q"
	{ 0x00, 0x00, 0x00, 0x70, 0x90, 0x90, 0x70, 0x10 },
	// code=114, ascii="r"
	{ 0x00, 0x00, 0x00, 0x50, 0x60, 0x40, 0x40, 0x00 },
	// code=115, ascii="s"
	{ 0x00, 0x00, 0x00, 0x70, 0xC0, 0x30, 0xE0, 0x00 },
	// code=116, ascii="t"
	{ 0x00, 0x00, 0x40, 0xF0, 0x40, 0x40, 0x30, 0x00 },
	// code=117, ascii="u"
	{ 0x00, 0x00, 0x00, 0x90, 0x90, 0x90, 0x70, 0x00 },
	// code=118, ascii="v"
	{ 0x00, 0x00, 0x00, 0x90, 0x90, 0x60, 0x60, 0x00 },
	// code=119, ascii="w"
	{ 0x00, 0x00, 0x00, 0x90, 0x90, 0xF0, 0x90, 0x00 },
	// code=120, ascii="x"
	{ 0x00, 0x00, 0x00, 0x90, 0x60, 0x60, 0x90, 0x00 },
	// code=121, ascii="y"
	{ 0x00, 0x00, 0x00, 0x90, 0x90, 0x70, 0x10, 0x60 },
	// code=122, ascii="z"
	{ 0x00, 0x00, 0x00, 0xF0, 0x20, 0x40, 0xF0, 0x00 },
	// code=123, ascii="{"
	{ 0x00, 0x10, 0x20, 0x40, 0x40, 0x20, 0x10, 0x00 },
	// code=124, ascii="|"
	{ 0x00, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00 },
	// code=125, ascii="}"
	{ 0x00, 0x40, 0x20, 0x10, 0x10, 0x20, 0x40, 0x00 },
	// code=126, ascii="~"
	{ 0x00, 0x00, 0x00, 0x50, 0xA0, 0x00, 0x00, 0x00 },
	// code=127, ascii="^?"
	{ 0x00, 0x00, 0x00, 0x20, 0x50, 0x88, 0xF8, 0x00 },
	// code=128, ascii="!^@"
	{ 0x00, 0x00, 0x70, 0x80, 0x80, 0x80, 0x70, 0x20 },
	// code=129, ascii="!^A"
	{ 0x00, 0x50, 0x00, 0x90, 0x90, 0x90, 0x70, 0x00 },
	// code=130, ascii="!^B"
	{ 0x10, 0x20, 0x00, 0x60, 0xF0, 0x80, 0x70, 0x00 },
	// code=131, ascii="!^C"
	{ 0x20, 0x50, 0x00, 0xC0, 0x20, 0xA0, 0x50, 0x00 },
	// code=132, ascii="!^D"
	{ 0x00, 0x50, 0x00, 0xC0, 0x20, 0x60, 0xB0, 0x00 },
	// code=133, ascii="!^E"
	{ 0x40, 0x20, 0x00, 0xC0, 0x20, 0x60, 0xB0, 0x00 },
	// code=134, ascii="!^F"
	{ 0x00, 0x20, 0x00, 0xC0, 0x20, 0x60, 0xB0, 0x00 },
	// code=135, ascii="!^G"
	{ 0x00, 0x00, 0x00, 0x30, 0x40, 0x40, 0x30, 0x20 },
	// code=136, ascii="!^H"
	{ 0x20, 0x50, 0x00, 0x60, 0xF0, 0x80, 0x70, 0x00 },
	// code=137, ascii="!^I"
	{ 0x00, 0x50, 0x00, 0x60, 0xF0, 0x80, 0x70, 0x00 },
	// code=138, ascii="!^J"
	{ 0x40, 0x20, 0x00, 0x60, 0xF0, 0x80, 0x70, 0x00 },
	// code=139, ascii="!^K"
	{ 0x00, 0x50, 0x00, 0x60, 0x20, 0x20, 0x70, 0x00 },
	// code=140, ascii="!^L"
	{ 0x20, 0x50, 0x00, 0x60, 0x20, 0x20, 0x70, 0x00 },
	// code=141, ascii="!^M"
	{ 0x40, 0x20, 0x00, 0x60, 0x20, 0x20, 0x70, 0x00 },
	// code=142, ascii="!^N"
	{ 0xA0, 0x00, 0x60, 0x90, 0xF0, 0x90, 0x90, 0x00 },
	// code=143, ascii="!^O"
	{ 0x20, 0x00, 0x60, 0x90, 0xF0, 0x90, 0x90, 0x00 },
	// code=144, ascii="!^P"
	{ 0x10, 0x20, 0xF0, 0x80, 0xE0, 0x80, 0xF0, 0x00 },
	// code=145, ascii="!^Q"
	{ 0x00, 0x00, 0x00, 0xD8, 0x78, 0xE0, 0xB8, 0x00 },
	// code=146, ascii="!^R"
	{ 0x00, 0x00, 0x70, 0xA0, 0xF0, 0xA0, 0xB0, 0x00 },
	// code=147, ascii="!^S"
	{ 0x20, 0x50, 0x00, 0x60, 0x90, 0x90, 0x60, 0x00 },
	// code=148, ascii="!^T"
	{ 0x00, 0x50, 0x00, 0x60, 0x90, 0x90, 0x60, 0x00 },
	// code=149, ascii="!^U"
	{ 0x40, 0x20, 0x00, 0x60, 0x90, 0x90, 0x60, 0x00 },
	// code=150, ascii="!^V"
	{ 0x20, 0x50, 0x00, 0x90, 0x90, 0x90, 0x70, 0x00 },
	// code=151, ascii="!^W"
	{ 0x40, 0x20, 0x00, 0x90, 0x90, 0x90, 0x70, 0x00 },
	// code=152, ascii="!^X"
	{ 0x00, 0x50, 0x00, 0x90, 0x90, 0x70, 0x10, 0x60 },
	// code=153, ascii="!^Y"
	{ 0x00, 0x50, 0x00, 0x60, 0x90, 0x90, 0x60, 0x00 },
	// code=154, ascii="!^Z"
	{ 0x50, 0x00, 0x90, 0x90, 0x90, 0x90, 0x60, 0x00 },
	// code=155, ascii="!^["
	{ 0x00, 0x00, 0x20, 0x70, 0x80, 0x80, 0x70, 0x20 },
	// code=156, ascii="!^\"
	{ 0x00, 0x30, 0x50, 0x40, 0xE0, 0x40, 0xF0, 0x00 },
	// code=157, ascii="!^]"
	{ 0x00, 0xD8, 0x50, 0x50, 0x20, 0x70, 0x20, 0x00 },
	// code=158, ascii="!^^"
	{ 0x00, 0xC0, 0xA0, 0xB0, 0xF8, 0x90, 0x88, 0x00 },
	// code=159, ascii="!^_"
	{ 0x00, 0x30, 0x40, 0x40, 0xF0, 0x40, 0x40, 0x80 },
	// code=160, ascii="! "
	{ 0x20, 0x40, 0x00, 0xC0, 0x20, 0x60, 0xB0, 0x00 },
	// code=161, ascii="!!"
	{ 0x10, 0x20, 0x00, 0x60, 0x20, 0x20, 0x70, 0x00 },
	// code=162, ascii="!""
	{ 0x10, 0x20, 0x00, 0x60, 0x90, 0x90, 0x60, 0x00 },
	// code=163, ascii="!#"
	{ 0x10, 0x20, 0x00, 0x90, 0x90, 0x90, 0x70, 0x00 },
	// code=164, ascii="!$"
	{ 0x50, 0xA0, 0x00, 0xE0, 0x90, 0x90, 0x90, 0x00 },
	// code=165, ascii="!%"
	{ 0x50, 0xA0, 0x90, 0xD0, 0xD0, 0xB0, 0x90, 0x00 },
	// code=166, ascii="!&"
	{ 0x00, 0x20, 0x50, 0x30, 0x00, 0x70, 0x00, 0x00 },
	// code=167, ascii="!'"
	{ 0x00, 0x20, 0x50, 0x20, 0x00, 0x70, 0x00, 0x00 },
	// code=168, ascii="!("
	{ 0x00, 0x20, 0x00, 0x20, 0x40, 0x90, 0x60, 0x00 },
	// code=169, ascii="!)"
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x80, 0x00 },
	// code=170, ascii="!*"
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x08, 0x00 },
	// code=171, ascii="!+"
	{ 0x00, 0x80, 0x90, 0xA0, 0x58, 0x88, 0x38, 0x00 },
	// code=172, ascii="!,"
	{ 0x00, 0x88, 0x90, 0xA0, 0x48, 0x98, 0x38, 0x08 },
	// code=173, ascii="!-"
	{ 0x00, 0x20, 0x00, 0x20, 0x20, 0x70, 0x20, 0x00 },
	// code=174, ascii="!."
	{ 0x00, 0x00, 0x00, 0x00, 0x50, 0xA0, 0x50, 0x00 },
	// code=175, ascii="!/"
	{ 0x00, 0x00, 0x00, 0x00, 0xA0, 0x50, 0xA0, 0x00 },
	// code=176, ascii="!0"
	{ 0xA8, 0x50, 0xA8, 0x50, 0xA8, 0x50, 0xA8, 0x50 },
	// code=177, ascii="!1"
	{ 0xE8, 0x50, 0xB8, 0x50, 0xE8, 0x50, 0xB8, 0x50 },
	// code=178, ascii="!2"
	{ 0xD8, 0x70, 0xD8, 0x70, 0xD8, 0x70, 0xD8, 0x70 },
	// code=179, ascii="!3"
	{ 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20 },
	// code=180, ascii="!4"
	{ 0x20, 0x20, 0x20, 0xE0, 0x20, 0x20, 0x20, 0x20 },
	// code=181, ascii="!5"
	{ 0x20, 0x20, 0xE0, 0x20, 0xE0, 0x20, 0x20, 0x20 },
	// code=182, ascii="!6"
	{ 0x50, 0x50, 0x50, 0xD0, 0x50, 0x50, 0x50, 0x50 },
	// code=183, ascii="!7"
	{ 0x00, 0x00, 0x00, 0xF0, 0x50, 0x50, 0x50, 0x50 },
	// code=184, ascii="!8"
	{ 0x00, 0x00, 0xE0, 0x20, 0xE0, 0x20, 0x20, 0x20 },
	// code=185, ascii="!9"
	{ 0x50, 0x50, 0xD0, 0x10, 0xD0, 0x50, 0x50, 0x50 },
	// code=186, ascii="!:"
	{ 0x50, 0x50, 0x50, 0x50, 0x50, 0x50, 0x50, 0x50 },
	// code=187, ascii="!;"
	{ 0x00, 0x00, 0xF0, 0x10, 0xD0, 0x50, 0x50, 0x50 },
	// code=188, ascii="!<"
	{ 0x50, 0x50, 0xD0, 0x10, 0xF0, 0x00, 0x00, 0x00 },
	// code=189, ascii="!="
	{ 0x50, 0x50, 0x50, 0xF0, 0x00, 0x00, 0x00, 0x00 },
	// code=190, ascii="!>"
	{ 0x20, 0x20, 0xE0, 0x20, 0xE0, 0x00, 0x00, 0x00 },
	// code=191, ascii="!?"
	{ 0x00, 0x00, 0x00, 0xE0, 0x20, 0x20, 0x20, 0x20 },
	// code=192, ascii="!@"
	{ 0x20, 0x20, 0x20, 0x38, 0x00, 0x00, 0x00, 0x00 },
	// code=193, ascii="!A"
	{ 0x20, 0x20, 0x20, 0xF8, 0x00, 0x00, 0x00, 0x00 },
	// code=194, ascii="!B"
	{ 0x00, 0x00, 0x00, 0xF8, 0x20, 0x20, 0x20, 0x20 },
	// code=195, ascii="!C"
	{ 0x20, 0x20, 0x20, 0x38, 0x20, 0x20, 0x20, 0x20 },
	// code=196, ascii="!D"
	{ 0x00, 0x00, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00 },
	// code=197, ascii="!E"
	{ 0x20, 0x20, 0x20, 0xF8, 0x20, 0x20, 0x20, 0x20 },
	// code=198, ascii="!F"
	{ 0x20, 0x20, 0x38, 0x20, 0x38, 0x20, 0x20, 0x20 },
	// code=199, ascii="!G"
	{ 0x50, 0x50, 0x50, 0x58, 0x50, 0x50, 0x50, 0x50 },
	// code=200, ascii="!H"
	{ 0x50, 0x50, 0x58, 0x40, 0x78, 0x00, 0x00, 0x00 },
	// code=201, ascii="!I"
	{ 0x00, 0x00, 0x78, 0x40, 0x58, 0x50, 0x50, 0x50 },
	// code=202, ascii="!J"
	{ 0x50, 0x50, 0xD8, 0x00, 0xF8, 0x00, 0x00, 0x00 },
	// code=203, ascii="!K"
	{ 0x00, 0x00, 0xF8, 0x00, 0xD8, 0x50, 0x50, 0x50 },
	// code=204, ascii="!L"
	{ 0x50, 0x50, 0x58, 0x40, 0x58, 0x50, 0x50, 0x50 },
	// code=205, ascii="!M"
	{ 0x00, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00, 0x00 },
	// code=206, ascii="!N"
	{ 0x50, 0x50, 0xD8, 0x00, 0xD8, 0x50, 0x50, 0x50 },
	// code=207, ascii="!O"
	{ 0x20, 0x20, 0xF8, 0x00, 0xF8, 0x00, 0x00, 0x00 },
	// code=208, ascii="!P"
	{ 0x50, 0x50, 0x50, 0xF8, 0x00, 0x00, 0x00, 0x00 },
	// code=209, ascii="!Q"
	{ 0x00, 0x00, 0xF8, 0x00, 0xF8, 0x20, 0x20, 0x20 },
	// code=210, ascii="!R"
	{ 0x00, 0x00, 0x00, 0xF8, 0x50, 0x50, 0x50, 0x50 },
	// code=211, ascii="!S"
	{ 0x50, 0x50, 0x50, 0x78, 0x00, 0x00, 0x00, 0x00 },
	// code=212, ascii="!T"
	{ 0x20, 0x20, 0x38, 0x20, 0x38, 0x00, 0x00, 0x00 },
	// code=213, ascii="!U"
	{ 0x00, 0x00, 0x38, 0x20, 0x38, 0x20, 0x20, 0x20 },
	// code=214, ascii="!V"
	{ 0x00, 0x00, 0x00, 0x78, 0x50, 0x50, 0x50, 0x50 },
	// code=215, ascii="!W"
	{ 0x50, 0x50, 0x50, 0xF8, 0x50, 0x50, 0x50, 0x50 },
	// code=216, ascii="!X"
	{ 0x20, 0x20, 0xF8, 0x20, 0xF8, 0x20, 0x20, 0x20 },
	// code=217, ascii="!Y"
	{ 0x20, 0x20, 0x20, 0xE0, 0x00, 0x00, 0x00, 0x00 },
	// code=218, ascii="!Z"
	{ 0x00, 0x00, 0x00, 0x38, 0x20, 0x20, 0x20, 0x20 },
	// code=219, ascii="!["
	{ 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8 },
	// code=220, ascii="!\"
	{ 0x00, 0x00, 0x00, 0x00, 0xF8, 0xF8, 0xF8, 0xF8 },
	// code=221, ascii="!]"
	{ 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0 },
	// code=222, ascii="!^"
	{ 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18 },
	// code=223, ascii="!_"
	{ 0xF8, 0xF8, 0xF8, 0xF8, 0x00, 0x00, 0x00, 0x00 },
	// code=224, ascii="!`"
	{ 0x00, 0x00, 0x00, 0x68, 0x90, 0x90, 0x68, 0x00 },
	// code=225, ascii="!a"
	{ 0x00, 0x60, 0x90, 0xF0, 0x90, 0x90, 0xE0, 0x80 },
	// code=226, ascii="!b"
	{ 0x00, 0x70, 0x40, 0x40, 0x40, 0x40, 0x40, 0x00 },
	// code=227, ascii="!c"
	{ 0x00, 0x00, 0x70, 0x50, 0x50, 0x50, 0x50, 0x00 },
	// code=228, ascii="!d"
	{ 0x00, 0xF8, 0x48, 0x20, 0x40, 0x88, 0xF8, 0x00 },
	// code=229, ascii="!e"
	{ 0x00, 0x00, 0x78, 0x90, 0x90, 0x90, 0x60, 0x00 },
	// code=230, ascii="!f"
	{ 0x00, 0x00, 0x00, 0x90, 0x90, 0x90, 0xE8, 0x80 },
	// code=231, ascii="!g"
	{ 0x00, 0x98, 0x50, 0x20, 0x20, 0x20, 0x20, 0x00 },
	// code=232, ascii="!h"
	{ 0x00, 0x20, 0x20, 0x70, 0x88, 0x70, 0x20, 0x20 },
	// code=233, ascii="!i"
	{ 0x00, 0x00, 0x70, 0x88, 0xF8, 0x88, 0x70, 0x00 },
	// code=234, ascii="!j"
	{ 0x00, 0x00, 0x70, 0x88, 0x88, 0x50, 0xD8, 0x00 },
	// code=235, ascii="!k"
	{ 0x60, 0x80, 0x40, 0x60, 0x90, 0x90, 0x60, 0x00 },
	// code=236, ascii="!l"
	{ 0x00, 0x00, 0x00, 0x70, 0xA8, 0xA8, 0x70, 0x00 },
	// code=237, ascii="!m"
	{ 0x00, 0x00, 0x08, 0x70, 0xA8, 0x48, 0xB0, 0x00 },
	// code=238, ascii="!n"
	{ 0x00, 0x30, 0x40, 0x70, 0x40, 0x40, 0x30, 0x00 },
	// code=239, ascii="!o"
	{ 0x00, 0x60, 0x90, 0x90, 0x90, 0x90, 0x90, 0x00 },
	// code=240, ascii="!p"
	{ 0x00, 0x00, 0xF0, 0x00, 0xF0, 0x00, 0xF0, 0x00 },
	// code=241, ascii="!q"
	{ 0x00, 0x00, 0x20, 0xF8, 0x20, 0x00, 0xF8, 0x00 },
	// code=242, ascii="!r"
	{ 0x00, 0x40, 0x20, 0x10, 0x20, 0x40, 0xF0, 0x00 },
	// code=243, ascii="!s"
	{ 0x00, 0x10, 0x20, 0x40, 0x20, 0x10, 0x70, 0x00 },
	// code=244, ascii="!t"
	{ 0x00, 0x00, 0x18, 0x28, 0x20, 0x20, 0x20, 0x20 },
	// code=245, ascii="!u"
	{ 0x20, 0x20, 0x20, 0x20, 0xA0, 0xC0, 0x00, 0x00 },
	// code=246, ascii="!v"
	{ 0x00, 0x00, 0x60, 0x00, 0xF0, 0x00, 0x60, 0x00 },
	// code=247, ascii="!w"
	{ 0x00, 0x00, 0x50, 0xA0, 0x00, 0x50, 0xA0, 0x00 },
	// code=248, ascii="!x"
	{ 0x00, 0x00, 0x20, 0x50, 0x20, 0x00, 0x00, 0x00 },
	// code=249, ascii="!y"
	{ 0x00, 0x00, 0x00, 0x60, 0x60, 0x00, 0x00, 0x00 },
	// code=250, ascii="!z"
	{ 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00 },
	// code=251, ascii="!{"
	{ 0x00, 0x00, 0x18, 0x10, 0x20, 0xA0, 0x40, 0x00 },
	// code=252, ascii="!|"
	{ 0x00, 0x60, 0x50, 0x50, 0x00, 0x00, 0x00, 0x00 },
	// code=253, ascii="!}"
	{ 0x00, 0x60, 0x10, 0x20, 0x70, 0x00, 0x00, 0x00 },
	// code=254, ascii="!~"
	{ 0x00, 0x00, 0x00, 0x70, 0x70, 0x70, 0x00, 0x00 },
	// code=255, ascii="!^�"
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
};


#define CVUI_IMPLEMENTATION

#define WINDOW_NAME "3piSimulator"
#define START_BUTTON_X 650
#define START_BUTTON_Y 425
#define RESET_BUTTON_X 900 - 50 - reset_button.cols
#define RESET_BUTTON_Y 425

#define A_BUTTON_X ((1500 - model3pi.cols) / 2)
#define A_BUTTON_Y (30 + 133)
#define B_BUTTON_X (((1500 - model3pi.cols) / 2) + 54)
#define B_BUTTON_Y (30 + 133)
#define C_BUTTON_X (((1500 - model3pi.cols) / 2) + 92)
#define C_BUTTON_Y (30 + 133)

#define POLOLU_DIAMETER 30
#define VEL_CONST 2.8

using namespace cv;
using namespace std;

extern void setup();
extern void loop();

Mat background;
Mat frame;
Mat LCD;
Mat LCDscaled;
VideoWriter outputVideo;
int print_count = 0;

Mat mini3pi;
Mat temp3pi;
Mat mini3piMask;
Mat model3pi;
Mat model3piMask;
Mat button;
Mat reset_button;
Mat start_button;
Mat pause_button;
Mat sensorLED[2];
Mat ledMask;

Point mouse;

Timer timer;

float x = 0;
float y = 0;
float newx, newy;
float theta = 0;

bool holding;
bool reset;
bool paused = false;
bool simulation_has_started = 0;

void readImages();
void simulateAndShow();
void simulate();
void test(float newx, float newy, float angle);
void draw3piRobot(float x, float y, float rotation);
void draw3piRobot();
void draw3piModel();
void drawButtons();
void drawSensorsLEDs();
void drawLCD();
void drawChrono();
void CallBackFunc(int events, int x, int y, int flags, void *userdata);
void buttonPressed(char choosed_button);
void buttonReleased(char choosed_button);
void resetAll();
void printScreen();
void delay(int millisseconds);

void drawLCDCharacter(int x, int y, char c) {
	//static bool done = false;

	if (x < 0 || x >= 8 || y < 0 || y >= 2) return;

	int localOffsetx = x * 6;
	int localOffsety = y * 9;

	for (int i = 0; i < 8; i++) {
		char line = OrangutanLCD::ascii[c][i];
		char masks[5] = { { 0b10000000 }, { 0b01000000 }, { 0b00100000 }, { 0b00010000 }, { 0b00001000 } };
		for (int j = 0; j < 5; j++) {
			if (masks[j] & line) {
				LCD.at<Vec3b>(localOffsety + i, localOffsetx + j) = Vec3b(0, 0, 0);
			}
			//if (!done) printf("%d\n", masks);
			//masks[j] >>= 1;
		}
	}
	//done = true;
}

void drawLCD() {

	LCD = Scalar(80, 208, 146);

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 8; j++) {
			drawLCDCharacter(j, i, OrangutanLCD::content[i][j]);
		}
	}

	resize(LCD, LCDscaled, Size(47 * 2, 17 * 2));
	Rect WhereRec(OrangutanLCD::offsetx - 18, OrangutanLCD::offsety - 2, LCDscaled.cols, LCDscaled.rows);
	LCDscaled.copyTo(frame(WhereRec));
}

void main() {
	/*
		implementacao de JOMA: line = 0b{000}00000
		nova implementacao:    line = 0b00000{000}
		(os bits em {chaves} sao os desprezados no drawLCDCharachter())

		solucoes:
			1. fazer line >>= 3 para todas das linhas (custoso)
			2. fazer line >>= 1 para todas as linhas e mask = 0b0100000 no
				drawLCDCharacter() (menos custoso)

			3. fazer um array com 5 masks com o elemento i+1 = (i >>= 1) a
				partir do 0b10000000 (IMPLEMENTADA)

		nota:	
			- fazer mask = 0b1000000 seria o ideal, mas nao funciona
				(ainda nao sei pq)
	*/	

	int fourcc = CV_FOURCC('M', 'J', 'P', 'G');
	outputVideo.open("output.avi", fourcc, 25, Size(900,600), true);

	if (!outputVideo.isOpened()) {
		cerr << "Could not open the output video file for write\n";
		return;
	}

	OrangutanLCD::x = 0;
	OrangutanLCD::y = 0;

	OrangutanLCD::print("3Pi SimulatorCBA");
	//OrangutanLCD::print("latorCBA");

	OrangutanLCD::offsetx = 647 + 75;
	OrangutanLCD::offsety = 30 + 75;
	
	readImages();
	cvui::init(WINDOW_NAME);
	frame = Mat(Size(900, 600), CV_8UC3);
	LCD = Mat(Size(47, 17), CV_8UC3);

	OrangutanMotors::vleft = 0;
	OrangutanMotors::vright = 0;

	std::thread simulatorThread(simulateAndShow);

	setup(); // extern funtion (SimpleLineFollower.cpp)
	timer.startCount();
	simulation_has_started = true;
	while (true) {
		loop();
		Sleep(20);
	}
	timer.stopCount();
	simulatorThread.join();
}

void readImages() {
	background = imread("background.png", 1);

	// 3pi robot
	mini3pi = imread("mini3pi.png");
	mini3piMask.create(mini3pi.size(), CV_8UC1);
	mini3piMask = 0; 
	circle(mini3piMask, Point(mini3pi.cols / 2, mini3pi.rows / 2), POLOLU_DIAMETER/2, Scalar(255, 255, 255), -1);

	// 3pi shown in the user interface
	model3pi = imread("pololu_desenho.png");
	model3piMask.create(model3pi.size(), CV_8UC1);
	model3piMask = 0;
	circle(model3piMask, Point(model3pi.cols / 2, model3pi.rows / 2), model3pi.cols / 2 - 13, Scalar(255, 255, 255), -1);

	// A, B C buttons
	button = imread("botao.png");
	
	// lit and unlit LEDs
	sensorLED[0] = imread("led48_yellow_0.png");
	sensorLED[1] = imread("led48_yellow_1.png");
	ledMask.create(sensorLED[0].size(), CV_8UC1);
	ledMask = 0;
	circle(ledMask, Point(ledMask.cols / 2, ledMask.rows / 2), ledMask.cols / 2, Scalar(255, 255, 255), -1);

	// start, pause and reset buttons
	reset_button = imread("reset.png");
	start_button = imread("start.png");
	pause_button = imread("pause.png");
}

void simulateAndShow() {
	while (true) {
		simulate();

		setMouseCallback(WINDOW_NAME, CallBackFunc, NULL);

		frame = Scalar(49, 52, 49);

		cvui::image(frame, 0, 0, background);
		draw3piModel();
		draw3piRobot();
		drawButtons();
		drawSensorsLEDs();
		drawLCD();
		drawChrono();

		cvui::update();

		outputVideo.write(frame);

		imshow(WINDOW_NAME, frame);

		int c = waitKey(10);
		if (c == 'A' || c == 'a') OrangutanPushbuttons::buttonAState = 1;
		if (c == 'B' || c == 'b') OrangutanPushbuttons::buttonBState = 1;
		if (c == 'C' || c == 'c') OrangutanPushbuttons::buttonCState = 1;
		if (c == 'P' || c == 'p') printScreen();
		if (c == 27) {
			outputVideo.release();
			exit(0);
		}
	}
}

void simulate() {

	float dt = 0.01;
	float vright;
	float vleft;

	if (holding) {
		x = mouse.x - 300;
		y = 300 - mouse.y;
		vright = 0;
		vleft = 0;
	}
	else if (!paused) {
		if (abs(OrangutanMotors::vright) <= 10.9 / 2.9) {
			vright = 0;
		}
		else if (OrangutanMotors::vright > 0) {
			vright = OrangutanMotors::vright * 2.9 - 10.9;
		}
		else {
			vright = OrangutanMotors::vright * 2.9 + 10.9;
		}

		if (abs(OrangutanMotors::vleft) < 10.9 / 2.9) {
			vleft = 0;
		}
		else if (OrangutanMotors::vleft > 0) {
			vleft = OrangutanMotors::vleft * 2.9 - 10.9;
		}
		else {
			vleft = OrangutanMotors::vleft * 2.9 + 10.9;

		}

		float diameter = POLOLU_DIAMETER;

		float vxr = -(vleft + vright) / 2; // x velocity
		float w = (vright - vleft) / diameter; // angular velocity

		Mat newPos;
		newPos.create(3, 1, CV_32FC1);

		float newTheta = theta + w * dt;
		Mat RotationMatrix;
		RotationMatrix.create(3, 3, CV_32FC1);
		RotationMatrix.at<float>(0, 0) = cos(newTheta);
		RotationMatrix.at<float>(0, 1) = -sin(newTheta);
		RotationMatrix.at<float>(0, 2) = 0;
		RotationMatrix.at<float>(1, 0) = sin(newTheta);
		RotationMatrix.at<float>(1, 1) = cos(newTheta);
		RotationMatrix.at<float>(1, 2) = 0;
		RotationMatrix.at<float>(2, 0) = 0;
		RotationMatrix.at<float>(2, 1) = 0;
		RotationMatrix.at<float>(2, 2) = 1;

		Mat diffPoseAtRobot;
		diffPoseAtRobot.create(3, 1, CV_32FC1);
		diffPoseAtRobot.at<float>(0, 0) = vxr;
		diffPoseAtRobot.at<float>(1, 0) = 0;
		diffPoseAtRobot.at<float>(2, 0) = w;

		Mat oldPose;
		oldPose.create(3, 1, CV_32FC1);
		oldPose.at<float>(0, 0) = x;
		oldPose.at<float>(1, 0) = y;
		oldPose.at<float>(2, 0) = theta;

		gemm(RotationMatrix, diffPoseAtRobot, dt, oldPose, 1, newPos);

		x = newPos.at<float>(0, 0);
		y = newPos.at<float>(1, 0);
		theta = newPos.at<float>(2, 0);
	}
}

void test(float newx, float newy, float angle) {
	float t = angle * 3.14 / 180;

	Mat rot;
	rot.create(2, 2, CV_32FC1);
	rot.at<float>(0, 0) = cos(t);
	rot.at<float>(0, 1) = -sin(t);
	rot.at<float>(1, 0) = sin(t);
	rot.at<float>(1, 1) = cos(t);

	Mat originalSensorPositions;
	originalSensorPositions.create(2, 5, CV_32FC1);
	originalSensorPositions.at<float>(0, 4) = -13.40277778 * 0.55;
	originalSensorPositions.at<float>(1, 4) = 19.93055556 * 0.55;
	originalSensorPositions.at<float>(0, 3) = -5.277777778 * 0.55;
	originalSensorPositions.at<float>(1, 3) = 23.40277778 * 0.55;
	originalSensorPositions.at<float>(0, 2) = 0;
	originalSensorPositions.at<float>(1, 2) = 23.95833333 * 0.55;
	originalSensorPositions.at<float>(0, 1) = 5.208333333 * 0.55;
	originalSensorPositions.at<float>(1, 1) = 23.40277778 * 0.55;
	originalSensorPositions.at<float>(0, 0) = 13.40277778 * 0.55;
	originalSensorPositions.at<float>(1, 0) = 19.72222222 * 0.55;

	Mat destSensorPositions;
	gemm(rot, originalSensorPositions, 1, 0, 0, destSensorPositions);

	for (int j = 0; j < destSensorPositions.cols; j++) {
		Point p(round(newx + destSensorPositions.at<float>(0, j) + POLOLU_DIAMETER/2), round(newy + POLOLU_DIAMETER/2 - destSensorPositions.at<float>(1, j)));
		circle(frame, p, 1, Scalar(0, 255, 0), -1);
		Vec3b color = background.at<Vec3b>(p);
		if (color.val[0] < 10 && color.val[1] < 10 && color.val[2] < 10) {
			Pololu3pi::sensors[j] = 1;
		}
		else {
			Pololu3pi::sensors[j] = 0;
		}
	}
}

void draw3piRobot(float x, float y, float rotation) {

	if (temp3pi.empty()) {
		temp3pi = mini3pi.clone();
	}

	Mat rot = getRotationMatrix2D(Point(temp3pi.cols / 2, temp3pi.rows / 2), rotation, 1);
	warpAffine(mini3pi, temp3pi, rot, mini3pi.size());

	if (holding) {
		newx = x - mini3pi.cols / 2;
		newy = y - mini3pi.rows / 2;
	}
	else {
		// 300 em relacao ao frame
		newx = x + 300 - mini3pi.cols / 2;
		newy = 300 - y - mini3pi.rows / 2;
	}
	Rect WhereRec(newx, newy, mini3pi.cols, mini3pi.rows);
	temp3pi.copyTo(frame(WhereRec), mini3piMask);

	test(newx, newy, rotation - 90);
}

void draw3piRobot() {
	if (holding) {
		draw3piRobot(mouse.x, mouse.y, theta * 180 / 3.14);
	}
	else {
		draw3piRobot(x, y, theta * 180 / 3.14);
	}
}

void draw3piModel() {
	int x;
	x = (1500 - model3pi.cols) / 2; // equation to center the 3pimodel
									//cvui::image(frame, x, 30, model3pi);
	Rect WhereRec(x, 30, model3pi.cols, model3pi.rows);
	model3pi.copyTo(frame(WhereRec), model3piMask);
}

void drawButtons() {

	// A, B, C
	cvui::image(frame, ((1500 - model3pi.cols) / 2) + 54, 30 + 133, button);
	cvui::image(frame, ((1500 - model3pi.cols) / 2) + 92, 30 + 133, button);
	cvui::image(frame, ((1500 - model3pi.cols) / 2) + 130, 30 + 133, button);

	// start, pause and reset 
	if (paused)
		cvui::image(frame, START_BUTTON_X, START_BUTTON_Y, start_button);
	else
		cvui::image(frame, START_BUTTON_X, START_BUTTON_Y, pause_button);
	cvui::image(frame, RESET_BUTTON_X, RESET_BUTTON_Y, reset_button);
}

void drawSensorsLEDs() {
	
	for (int i = 0; i < 5; i++) {
		Rect WhereRec((1500 - sensorLED[0].cols) / 2 - 20 * (i - 2), 60, ledMask.cols, ledMask.rows);
		if (Pololu3pi::sensors[i] == 1) {
			sensorLED[1].copyTo(frame(WhereRec), ledMask);
		}
		else {
			sensorLED[0].copyTo(frame(WhereRec), ledMask);
		}
	}
}

void drawChrono() {
	cvui::printf(frame, 675, 300, 1, 0xffffff, "%d'%2d\"%0d", timer.getMin(), timer.getSecs() % 60, timer.getMilliSecs() % 1000);
}

void CallBackFunc(int event, int x, int y, int flags, void* userdata) {
	Rect mouse_pos(x, y, 1, 1);
	char choosed_button;
	bool over_a_button = false;

	if (((mouse_pos & Rect(START_BUTTON_X, START_BUTTON_Y, start_button.cols, start_button.rows)) == mouse_pos)) {
		// start/pause
		choosed_button = 'p';
		over_a_button = true;
	}
	else if (((mouse_pos & Rect(RESET_BUTTON_X, RESET_BUTTON_Y, start_button.cols, start_button.rows)) == mouse_pos)) {
		// reset
		choosed_button = 'r';
		over_a_button = true;
	}
	else if (((mouse_pos & Rect(A_BUTTON_X, A_BUTTON_Y, start_button.cols, start_button.rows)) == mouse_pos)) {
		// A
		choosed_button = 'a';
		over_a_button = true;
	}
	else if (((mouse_pos & Rect(B_BUTTON_X, B_BUTTON_Y, start_button.cols, start_button.rows)) == mouse_pos)) {
		// B
		choosed_button = 'b';
		over_a_button = true;
	}
	else if (((mouse_pos & Rect(C_BUTTON_X, C_BUTTON_Y, start_button.cols, start_button.rows)) == mouse_pos)) {
		// C
		choosed_button = 'c';
		over_a_button = true;
	}
	else if (((mouse_pos & Rect(newx, newy, mini3pi.cols, mini3pi.rows)) == mouse_pos)) {
		// 3piRobot at simulation (drag functionality)
		choosed_button = '3';
		over_a_button = true;
	}
	else {
		over_a_button = false;
	}

	if (event == EVENT_LBUTTONDOWN) {
		// left mouse button clicked
		if (over_a_button) buttonPressed(choosed_button);
	}
	else if (event == EVENT_LBUTTONUP) {
		// left mouse button released
		if (over_a_button) buttonReleased(choosed_button);
	}
	else if (event == EVENT_MOUSEMOVE) {
		//cout << "Mouse move over the window - position (" << x << ", " << y << ")" << endl;
	}

	mouse = Point(x, y);
	//cout << "Mouse move over the window - position (" << mouse.x - 300 << ", " << 300 - mouse.y << ")" << endl;
}

void buttonPressed(char choosed_button) {
	switch (choosed_button) {
	case 'a':
		OrangutanPushbuttons::buttonAState = 1;
		break;
	case 'b':
		OrangutanPushbuttons::buttonBState = 1;
		break;
	case 'c':
		OrangutanPushbuttons::buttonCState = 1;
		break;
	case 'r':
		resetAll();
		break;
	case 'p':
		if (simulation_has_started) {
			paused = !paused;
			if (!paused) timer.startCount();
		}
		break;
	case '3':
		holding = true;
		paused = true;
	default:
		break;
	}

	if (paused) timer.stopCount();
}

void buttonReleased(char choosed_button) {
	switch (choosed_button) {
	case 'a':
		OrangutanPushbuttons::buttonAState = 0;
		break;
	case 'b':
		OrangutanPushbuttons::buttonBState = 0;
		break;
	case 'c':
		OrangutanPushbuttons::buttonCState = 0;
		break;
	case '3':
		holding = false;
		break;
	default:
		break;
	}
}

void resetAll() {
	x = 0;
	y = 0;
	theta = 0;
	timer.resetCount();
	drawChrono();
	paused = true;
}

void printScreen() {
	stringstream ss;
	string name = "print_";
	string type = ".png";

	ss << "prints/" << name << print_count << type;

	string path = ss.str();
	ss.str("");

	imwrite(path, frame);
	print_count++;
}

void delay(int millisseconds) {
	Sleep(millisseconds);
}

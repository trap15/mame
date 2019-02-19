// license:BSD-3-Clause
// copyright-holders:Kevin Horton,Jonathan Gevaryahu,Sandro Ronco,hap
// thanks-to:Berger,yoyo_chessboard
/******************************************************************************

    Fidelity Electronics Z80 based board driver
    for 6502 based boards, see drivers/fidel6502.cpp
    for 68000 based boards, see drivers/fidel68k.cpp

    NOTE: MAME doesn't include a generalized implementation for boardpieces yet,
    greatly affecting user playability of emulated electronic board games.
    As workaround for the chess games, use an external chess GUI on the side,
    such as Arena(in editmode).

    TODO:
    - Source organization is a big mess. Each machine family could be in its own
      sub driverclass, and separate files.
    - card games aren't playable without the barcode scanner
    - Z80 WAIT pin is not fully emulated, affecting VBRC speech busy state
    TODO:
    - Source organization is a big mess. Each machine family could be in its own
      sub driverclass, and separate files.
    - verify cpu speed and rom labels where unknown
    - improve EAS/SC12/etc CPU divider? it seems a little bit slower than the real machine.
      Currently, a dummy timer workaround is needed, or it's much worse.
      Is the problem here is due to timing of CPU addressbus changes? We can only 'sense'
      the addressbus at read or write accesses.
    - finish fphantom emulation

    Read the official manual(s) on how to play.

    Keypad legend:
    - RE: Reset
    - CL: Clear
    - EN: Enter
    - PB: Problem Mode
    - PV: Position Verification
    - LV: Playing Levels
    - TB: Take Back
    - DM: Display Move/Double Move
    - RV: Reverse
    - ST: Set/Stop
    - TM: Time

    Peripherals, compatible with various boards:
    - Fidelity Challenger Printer - thermal printer, MCU=D8048C243

    Program/data cartridges, for various boards, some cross-compatible:
    - CB9: Challenger Book Openings 1 - 8KB (label not known)
    - CB16: Challenger Book Openings 2 - 8+8KB 101-1042A01,02
	- *CG64: 64 Greatest Games
	- *EOA-EOE: Challenger Book Openings - Chess Encyclopedia A-E (5 modules)

    Board hardware descriptions below.
    Detailed RE work done by Kevin 'kevtris' Horton, except where noted

*******************************************************************************

Voice Chess Challenger (VCC) (version A and B?)
Advanced Voice Chess Challenger (UVC)
Grandmaster Voice Chess Challenger
Decorator Challenger (FCC)

(which share the same hardware)
----------------------
The CPU is a Z80 running at 4MHz.  The TSI chip runs at around 25KHz, using a
470K / 100pf RC network.  This system is very very basic, and is composed of just
the Z80, 4 ROMs, the TSI chip, and an 8255.

The Z80's interrupt inputs are all pulled to VCC, so no interrupts are used.

Reset is connected to a power-on reset circuit and a button on the keypad (marked RE).

The TSI chip connects to a 4K ROM.  All of the 'Voiced' Chess Challengers
use this same ROM  (three or four).  The later chess boards use a slightly different part
number, but the contents are identical.

Memory map (VCC):
-----------
0000-0FFF: 4K 2332 ROM 101-32013
1000-1FFF: 4K 2332 ROM VCC2
2000-2FFF: 4K 2332 ROM VCC3
4000-5FFF: 1K RAM (2114 SRAM x2)
6000-FFFF: empty

Memory map (UVC):
-----------
0000-1FFF: 8K 2364 ROM 101-64017
2000-2FFF: 4K 2332 ROM 101-32010
4000-5FFF: 1K RAM (2114 SRAM x2)
6000-FFFF: empty

Port map:
---------
00-03: 8255 port chip, mirrored over the 00-FF range; program accesses F4-F7

8255 connections:
-----------------
PA.0 - segment G, TSI A0 (W)
PA.1 - segment F, TSI A1 (W)
PA.2 - segment E, TSI A2 (W)
PA.3 - segment D, TSI A3 (W)
PA.4 - segment C, TSI A4 (W)
PA.5 - segment B, TSI A5 (W)
PA.6 - segment A, language latch Data (W)
PA.7 - TSI START line, language latch clock (W, see below)

PB.0 - dot commons (W)
PB.1 - NC
PB.2 - digit 0, bottom dot (W)
PB.3 - digit 1, top dot (W)
PB.4 - digit 2 (W)
PB.5 - digit 3 (W)
PB.6 - enable language switches (W, see below)
PB.7 - TSI BUSY line (R)

(button rows pulled up to 5V through 2.2K resistors)
PC.0 - button row 0, German language jumper (R)
PC.1 - button row 1, French language jumper (R)
PC.2 - button row 2, Spanish language jumper (R)
PC.3 - button row 3, special language jumper (R)
PC.4 - button column A (W)
PC.5 - button column B (W)
PC.6 - button column C (W)
PC.7 - button column D (W)

language switches:
------------------
When PB.6 is pulled low, the language switches can be read.  There are four.
They connect to the button rows.  When enabled, the row(s) will read low if
the jumper is present.  English only VCC's do not have the 367 or any pads stuffed.
The jumpers are labeled: French, German, Spanish, and special.

language latch:
---------------
There's an unstuffed 7474 on the board that connects to PA.6 and PA.7.  It allows
one to latch the state of A12 to the speech ROM.  The English version has the chip
missing, and a jumper pulling "A12" to ground.  This line is really a negative
enable.

To make the VCC multi-language, one would install the 74367 (note: it must be a 74367
or possibly a 74LS367.  A 74HC367 would not work since they rely on the input current
to keep the inputs pulled up), solder a piggybacked ROM to the existing English
speech ROM, and finally install a 7474 dual flipflop.

This way, the game can then detect which secondary language is present, and then it can
automatically select the correct ROM(s).  I have to test whether it will do automatic
determination and give you a language option on power up or something.


*******************************************************************************

Chess Challenger 10 (CCX)
-------------------
4 versions are known to exist: A,B,C,D. Strangely, version C(UCC10) has an 8080
instead of Z80. Chess Challenger 1,3 and 7 also run on very similar hardware.

This is an earlier hardware upon which the VCC and UVC above were based on;
The hardware is nearly the same; in fact the only significant differences are
the RAM being located in a different place, the lack of a speech chip, and
the connections to ports A and B on the PPI:

8255 connections:
-----------------
PA.0 - segment G (W)
PA.1 - segment F (W)
PA.2 - segment E (W)
PA.3 - segment D (W)
PA.4 - segment C (W)
PA.5 - segment B (W)
PA.6 - segment A (W)
PA.7 - 'beeper' direct speaker output (W)

The beeper is via a 556 timer, fixed-frequency at around 1300-1400Hz.
Not all hardware configurations include the beeper.

PB.0 - dot commons (W)
PB.1 - NC
PB.2 - digit 0, bottom dot (W)
PB.3 - digit 1, top dot (W)
PB.4 - digit 2 (W)
PB.5 - digit 3 (W)
PB.6 - NC
PB.7 - Mode select (cc3 vs cc10, R) - note: there is no CC3 with 16 buttons

(button rows pulled up to 5V through 2.2K resistors)
PC.0 - button row 0 (R)
PC.1 - button row 1 (R)
PC.2 - button row 2 (R)
PC.3 - button row 3 (R)
PC.4 - button column A (W)
PC.5 - button column B (W)
PC.6 - button column C (W)
PC.7 - button column D (W)


*******************************************************************************

Chess Challenger 7 (BCC)
------------------------
model CC7 is an older version
RE information from netlist by Berger

Zilog Z80A, 3.579MHz from XTAL
Z80 IRQ/NMI unused, no timer IC.
This is a cost-reduced design from CC10, no special I/O chips.

Backgammon Challenger (BKC) is the same PCB, with the speaker connection going
to the display panel instead.

Memory map:
-----------
0000-0FFF: 4K 2332 ROM CN19103N BCC-REVB.
2000-2FFF: ROM/RAM bus conflict!
3000-3FFF: 256 bytes RAM (2111 SRAM x2)
4000-FFFF: Z80 A14/A15 not connected

Port map (Write):
---------
D0-D3: digit select and keypad mux
D4: LOSE led
D5: CHECK led
A0-A2: NE591 A0-A2
D7: NE591 D (_C not used)
NE591 Q0-Q6: digit segments A-G
NE591 Q7: buzzer

Port map (Read):
---------
D0-D3: keypad row


*******************************************************************************

Voice Bridge Challenger (Model VBRC, later reissued as Model 7002)
and Bridge Challenger 3 (Model 7014)
(which both share the same* hardware)
--------------------------------
* The Bridge Challenger 3 does not actually have the 8 LEDs nor the
latches which operate them populated and the plastic indicator cap locations
are instead are covered by a piece of plastic, but they do work if manually
added.

This unit is similar in construction kinda to the chess challengers, however it
has an 8041 which does ALL of the system I/O.  The Z80 has NO IO AT ALL other than
what is performed through the 8041!

The main CPU is a Z80 running at 2.5MHz

INT connects to VCC (not used)
NMI connects to VCC (not used)
RST connects to power on reset, and reset button

The 8041 runs at 5MHz.

Memory Map:
-----------
0000-1FFF: 8K 101-64108 ROM
2000-3FFF: 8K 101-64109 ROM
4000-5FFF: 8K 101-64110 ROM
6000-7FFF: 1K of RAM (2114 * 2)
8000-DFFF: unused
E000-FFFF: write to TSI chip

NOTE: when the TSI chip is written to, the CPU IS STOPPED.  The CPU will run again
when the word is done being spoken.  This is because D0-D5 run to the TSI chip directly.

The TSI chip's ROM is 4K, and is marked 101-32118.  The clock is the same as the Chess
Challengers- 470K/100pf which gives a frequency around 25KHz or so.

Port Map:
---------
00-FF: 8041 I/O ports (A0 selects between the two)

8041 pinout:
------------
(note: columns are pulled up with 10K resistors)

P10 - column H, RD LED, VFD grid 0
P11 - column G, DB LED, VFD grid 1
P12 - column F, <>V LED, VFD grid 2
P13 - column E, ^V LED, VFD grid 3
P14 - column D, W LED, VFD grid 4
P15 - column C, S LED, VFD grid 5
P16 - column B, E LED, VFD grid 6
P17 - column A, N LED, VFD grid 7

P20 - I/O expander
P21 - I/O expander
P22 - I/O expander
P23 - I/O expander
P24 - row 0 through inverter
P25 - row 1 through inverter
P26 - row 2 through inverter
P27 - row 3 through inverter

PROG - I/O expander

T0 - optical card sensor (high = bright/reflective, low = dark/non reflective)
T1 - connects to inverter, then 5MHz/4

D8243C I/O expander:
--------------------
P4.0 - segment M
P4.1 - segment L
P4.2 - segment N
P4.3 - segment E

P5.0 - segment D
P5.1 - segment I
P5.2 - segment K
P5.3 - segment J

P6.0 - segment A
P6.1 - segment B
P6.2 - segment F
P6.3 - segment G

P7.0 - LED enable (high = LEDs can be lit.  low = LEDs will not light)
P7.1 - goes through inverter, to pads that are not used
P7.2 - segment C
P7.3 - segment H

button matrix:
--------------
the matrix is composed of 8 columns by 4 rows.

     A  B  C  D     E  F  G  H
     -------------------------
0-   RE xx CL EN    J  Q  K  A
1-   BR PB DB SC    7  8  9 10
2-   DL CV VL PL    3  4  5  6
3-   cl di he sp   NT  P  1  2

xx - speaker symbol
cl - clubs symbol
di - diamonds symbol
he - hearts symbol
sp - spades symbol

NOTE: RE is not wired into the matrix, and is run separately out.

There are 8 LEDs, and an 8 digit 14 segment VFD with commas and periods.
This display is the same one as can be found on the speak and spell.

       A       * comma
  ***********  *
 * *I  *J K* *
F*  *  *  *  *B
 *   * * *   *
  G**** *****H
 *   * * *   *
E*  *  *  *  *C
 * *N  *M L* *
  ***********  *decimal point
       D

The digits of the display are numbered left to right, 0 through 7 and are controlled
by the grids.  hi = grid on, hi = segment on.

A detailed description of the hardware can be found also in the patent 4,373,719.


*******************************************************************************

Voice Sensory Chess Challenger (VSC)
------------------------------------
The display/button/LED/speech technology is identical to Fidelity CSC.
Only the CPU board was changed.  As such, it works the same but is interfaced
to different port chips this time.

Hardware:
---------
On the board are 13 chips.

The CPU is a Z80A running at 3.9MHz, with 20K of ROM and 1K of RAM mapped.
I/O is composed of an 8255 triple port adaptor, and a Z80A PIO parallel I/O
interface.

There's the usual TSI S14001A speech synth with its requisite 4K ROM which is the
same as on the other talking chess boards.  The TSI chip is running at 26.37KHz.
It uses a 470K resistor and a 100pf capacitor.

The "perfect" clock would be 1/RC most likely (actually this will be skewed a tad by
duty cycle of the oscillator) which with those parts values gives 21.27KHz.  The
formula is probably more likely to be 1/1.2RC or so.

Rounding out the hardware are three driver chips for the LEDs, a 7404 inverter to
run the crystal osc, a 555 timer to generate a clock, and a 74138 selector.

NMI runs to a 555 oscillator that generates a 600Hz clock (measured: 598.9Hz.  It has a multiturn pot to adjust).
INT is pulled to 5V
RST connects to a power-on reset circuit

Memory map:
-----------
0000-1FFF: 8K ROM 101-64018
2000-3FFF: 8K ROM 101-64019 (also used on the sensory champ. chess challenger)
4000-5FFF: 4K ROM 101-32024
6000-7FFF: 1K of RAM (2114 * 2)
8000-FFFF: not used, maps to open bus

Port map:
---------
There's only two chips in the portmap, an 8255 triple port chip, and a Z80A PIO
parallel input/output device.

Decoding isn't performed using a selector, but instead address lines are used.

A2 connects to /CE on the 8255
A3 connects to /CE on the Z80A PIO

A0 connects to port A/B select on PIO & A0 of 8255
A1 connects to control/data select on PIO & A1 of 8255

So to enable only the 8255, you'd write/read to 08-0Bh for example
To enable only the PIO, you'd write/read to 04-07h for example.

writing to 00-03h will enable and write to BOTH chips, and reading 00-03h
will return data from BOTH chips (and cause a bus conflict).  The code probably
never does either of these things.

Likewise, writing/reading to 0Ch-0Fh will result in open bus, because neither chip's
enable line will be low.

This sequence repeats every 16 addresses.  So to recap:

00-03: both chips enabled (probably not used)
04-07: PIO enabled
08-0B: 8255 enabled
0C-0F: neither enabled

10-FF: mirrors of 00-0F.

Refer to the Sensory Champ. Chess Chall. for explanations of the below
I/O names and labels.  It's the same.

8255:
-----
PA.0 - segment D, TSI A0
PA.1 - segment E, TSI A1
PA.2 - segment F, TSI A2
PA.3 - segment A, TSI A3
PA.4 - segment B, TSI A4
PA.5 - segment C, TSI A5
PA.6 - segment G
PA.7 - segment H

PB.0 - LED row 1
PB.1 - LED row 2
PB.2 - LED row 3
PB.3 - LED row 4
PB.4 - LED row 5
PB.5 - LED row 6
PB.6 - LED row 7
PB.7 - LED row 8

PC.0 - LED column A, button column A, 7seg digit 1
PC.1 - LED column B, button column B, 7seg digit 2
PC.2 - LED column C, button column C, 7seg digit 3
PC.3 - LED column D, button column D, 7seg digit 4
PC.4 - LED column E, button column E
PC.5 - LED column F, button column F
PC.6 - LED column G, button column G
PC.7 - LED column H, button column H

Z80A PIO:
---------
PA.0 - button row 1
PA.1 - button row 2
PA.2 - button row 3
PA.3 - button row 4
PA.4 - button row 5
PA.5 - button row 6
PA.6 - button row 7
PA.7 - button row 8

PB.0 - button column I
PB.1 - button column J
PB.2 - hi/lo TSI speaker volume
PB.3 - violet wire
PB.4 - white wire (and TSI BUSY line)
PB.5 - selection jumper input (see below)
PB.6 - TSI start line
PB.7 - TSI ROM A12 line

selection jumpers:
------------------
These act like another row of buttons.  It is composed of two diode locations,
so there's up to 4 possible configurations.  My board does not have either diode
stuffed, so this most likely is "English".  I suspect it selects which language to use
for the speech synth.  Of course you need the other speech ROMs for this to function
properly.

Anyways, the two jumpers are connected to button columns A and B and the common
connects to Z80A PIO PB.5, which basically makes a 10th button row.  I would
expect that the software reads these once on startup only.

******************************************************************************/

#include "emu.h"
#include "includes/fidelbase.h"

#include "cpu/z80/z80.h"
#include "cpu/mcs48/mcs48.h"
#include "machine/clock.h"
#include "machine/i8255.h"
#include "machine/i8243.h"
#include "machine/z80pio.h"
#include "sound/beep.h"
#include "sound/volt_reg.h"
#include "speaker.h"

// internal artwork
#include "fidel_cc10.lh" // clickable
#include "fidel_bcc.lh" // clickable
#include "fidel_bkc.lh" // clickable
#include "fidel_dsc.lh" // clickable
#include "fidel_sc8.lh" // clickable
#include "fidel_vcc.lh" // clickable
#include "fidel_vbrc.lh" // clickable
#include "fidel_bv3.lh" // clickable
#include "fidel_vsc.lh" // clickable

// machine start/reset

void fidelbase_state::machine_start()
{
	// resolve handlers
	m_out_x.resolve();
	m_out_a.resolve();
	m_out_digit.resolve();

	// zerofill
	memset(m_display_state, 0, sizeof(m_display_state));
	memset(m_display_decay, 0, sizeof(m_display_decay));
	memset(m_display_segmask, 0, sizeof(m_display_segmask));

	m_inp_mux = 0;
	m_led_select = 0;
	m_led_data = 0;
	m_7seg_data = 0;
	m_speech_data = 0;
	m_speech_bank = 0;

	// register for savestates
	save_item(NAME(m_display_maxy));
	save_item(NAME(m_display_maxx));
	save_item(NAME(m_display_wait));

	save_item(NAME(m_display_state));
	save_item(NAME(m_display_decay));
	save_item(NAME(m_display_segmask));

	save_item(NAME(m_div_status));
	save_item(NAME(m_inp_mux));
	save_item(NAME(m_led_select));
	save_item(NAME(m_led_data));
	save_item(NAME(m_7seg_data));
	save_item(NAME(m_speech_data));
	save_item(NAME(m_speech_bank));
}

void fidelbase_state::machine_reset()
{
	m_div_status = ~0;
}



class scc_state : public fidelbase_state
{
public:
	scc_state(const machine_config &mconfig, device_type type, const char *tag) :
		fidelbase_state(mconfig, type, tag)
	{ }

	void scc(machine_config &config);

private:
	void main_map(address_map &map);
	void main_io(address_map &map);

	// I/O handlers
	DECLARE_READ8_MEMBER(input_r);
	DECLARE_WRITE8_MEMBER(control_w);
};


class dsc_state : public fidelbase_state
{
public:
	dsc_state(const machine_config &mconfig, device_type type, const char *tag) :
		fidelbase_state(mconfig, type, tag)
	{ }

	void dsc(machine_config &config);

private:
	void main_map(address_map &map);

	// I/O handlers
	void prepare_display();
	DECLARE_WRITE8_MEMBER(control_w);
	DECLARE_WRITE8_MEMBER(select_w);
	DECLARE_READ8_MEMBER(input_r);
};


class bcc_state : public fidelbase_state
{
public:
	bcc_state(const machine_config &mconfig, device_type type, const char *tag) :
		fidelbase_state(mconfig, type, tag)
	{ }

	void bcc(machine_config &config);
	void bkc(machine_config &config);

private:
	void main_map(address_map &map);
	void main_io(address_map &map);

	// I/O handlers
	DECLARE_READ8_MEMBER(input_r);
	DECLARE_WRITE8_MEMBER(control_w);
};


class vsc_state : public fidelbase_state
{
public:
	vsc_state(const machine_config &mconfig, device_type type, const char *tag) :
		fidelbase_state(mconfig, type, tag),
		m_z80pio(*this, "z80pio"),
		m_ppi8255(*this, "ppi8255")
	{ }

	void vsc(machine_config &config);

private:
	// devices/pointers
	required_device<z80pio_device> m_z80pio;
	required_device<i8255_device> m_ppi8255;

	void main_map(address_map &map);
	void main_io(address_map &map);
	DECLARE_READ8_MEMBER(main_io_trampoline_r);
	DECLARE_WRITE8_MEMBER(main_io_trampoline_w);

	// I/O handlers
	void prepare_display();
	DECLARE_READ8_MEMBER(speech_r);
	DECLARE_WRITE8_MEMBER(ppi_porta_w);
	DECLARE_WRITE8_MEMBER(ppi_portb_w);
	DECLARE_WRITE8_MEMBER(ppi_portc_w);
	DECLARE_READ8_MEMBER(pio_porta_r);
	DECLARE_READ8_MEMBER(pio_portb_r);
	DECLARE_WRITE8_MEMBER(pio_portb_w);
};


class ccx_state : public fidelbase_state
{
public:
	ccx_state(const machine_config &mconfig, device_type type, const char *tag) :
		fidelbase_state(mconfig, type, tag),
		m_ppi8255(*this, "ppi8255"),
		m_beeper_off(*this, "beeper_off"),
		m_beeper(*this, "beeper")
	{ }

	void ccx(machine_config &config);

private:
	// devices/pointers
	required_device<i8255_device> m_ppi8255;
	required_device<timer_device> m_beeper_off;
	required_device<beep_device> m_beeper;

	TIMER_DEVICE_CALLBACK_MEMBER(beeper_off) { m_beeper->set_state(0); }

	void main_map(address_map &map);
	void main_io(address_map &map);

	// I/O handlers
	void prepare_display();
	DECLARE_WRITE8_MEMBER(ppi_porta_w);
	DECLARE_WRITE8_MEMBER(ppi_portb_w);
	DECLARE_READ8_MEMBER(ppi_portc_r);
	DECLARE_WRITE8_MEMBER(ppi_portc_w);
};


class card_state : public fidelbase_state
{
public:
	card_state(const machine_config &mconfig, device_type type, const char *tag) :
		fidelbase_state(mconfig, type, tag),
		m_mcu(*this, "mcu"),
		m_i8243(*this, "i8243")
	{ }

	void brc_base(machine_config &config);
	void ubc(machine_config &config);
	void vbrc(machine_config &config);
	void bv3(machine_config &config);

	virtual DECLARE_INPUT_CHANGED_MEMBER(reset_button) override;

private:
	// devices/pointers
	required_device<i8041_device> m_mcu;
	required_device<i8243_device> m_i8243;

	void main_map(address_map &map);
	void main_io(address_map &map);

	void prepare_display();
	DECLARE_WRITE8_MEMBER(speech_w);
	DECLARE_WRITE8_MEMBER(mcu_p1_w);
	DECLARE_READ8_MEMBER(mcu_p2_r);
	template<int P> void ioexp_port_w(uint8_t data);
};


class vcc_state : public fidelbase_state
{
public:
	vcc_state(const machine_config &mconfig, device_type type, const char *tag) :
		fidelbase_state(mconfig, type, tag),
		m_ppi8255(*this, "ppi8255")
	{ }

	void vcc(machine_config &config);

protected:
	virtual void machine_start() override;

private:
	// devices/pointers
	required_device<i8255_device> m_ppi8255;

	void main_map(address_map &map);
	void main_io(address_map &map);

	// I/O handlers
	void prepare_display();
	DECLARE_READ8_MEMBER(speech_r);
	DECLARE_WRITE8_MEMBER(ppi_porta_w);
	DECLARE_READ8_MEMBER(ppi_portb_r);
	DECLARE_WRITE8_MEMBER(ppi_portb_w);
	DECLARE_READ8_MEMBER(ppi_portc_r);
	DECLARE_WRITE8_MEMBER(ppi_portc_w);
};

void vcc_state::machine_start()
{
	fidelbase_state::machine_start();

	// game relies on RAM initialized filled with 1
	for (int i = 0; i < 0x400; i++)
		m_maincpu->space(AS_PROGRAM).write_byte(i + 0x4000, 0xff);
}





/***************************************************************************

  Helper Functions

***************************************************************************/

// The device may strobe the outputs very fast, it is unnoticeable to the user.
// To prevent flickering here, we need to simulate a decay.

void fidelbase_state::display_update()
{
	for (int y = 0; y < m_display_maxy; y++)
	{
		u32 active_state = 0;

		for (int x = 0; x <= m_display_maxx; x++)
		{
			// turn on powered segments
			if (m_display_state[y] >> x & 1)
				m_display_decay[y][x] = m_display_wait;

			// determine active state
			u32 ds = (m_display_decay[y][x] != 0) ? 1 : 0;
			active_state |= (ds << x);

			// output to y.x, or y.a when always-on
			if (x != m_display_maxx)
				m_out_x[y][x] = ds;
			else
				m_out_a[y] = ds;
		}

		// output to digity
		if (m_display_segmask[y] != 0)
			m_out_digit[y] = active_state & m_display_segmask[y];
	}
}

TIMER_DEVICE_CALLBACK_MEMBER(fidelbase_state::display_decay_tick)
{
	// slowly turn off unpowered segments
	for (int y = 0; y < m_display_maxy; y++)
		for (int x = 0; x <= m_display_maxx; x++)
			if (m_display_decay[y][x] != 0)
				m_display_decay[y][x]--;

	display_update();
}

void fidelbase_state::set_display_size(int maxx, int maxy)
{
	m_display_maxx = maxx;
	m_display_maxy = maxy;
}

void fidelbase_state::set_display_segmask(u32 digits, u32 mask)
{
	// set a segment mask per selected digit, but leave unselected ones alone
	for (int i = 0; i < 0x20; i++)
	{
		if (digits & 1)
			m_display_segmask[i] = mask;
		digits >>= 1;
	}
}

void fidelbase_state::display_matrix(int maxx, int maxy, u32 setx, u32 sety, bool update)
{
	set_display_size(maxx, maxy);

	// update current state
	u32 mask = (1 << maxx) - 1;
	for (int y = 0; y < maxy; y++)
		m_display_state[y] = (sety >> y & 1) ? ((setx & mask) | (1 << maxx)) : 0;

	if (update)
		display_update();
}


// generic input handlers

u16 fidelbase_state::read_inputs(int columns)
{
	u16 ret = 0;

	// read selected input rows
	for (int i = 0; i < columns; i++)
		if (m_inp_mux >> i & 1)
			ret |= m_inp_matrix[i]->read();

	return ret;
}


// cartridge

DEVICE_IMAGE_LOAD_MEMBER(fidelbase_state, scc_cartridge)
{
	u32 size = m_cart->common_get_size("rom");

	// max size is 16KB?
	if (size > 0x4000)
	{
		image.seterror(IMAGE_ERROR_UNSPECIFIED, "Invalid file size");
		return image_init_result::FAIL;
	}

	m_cart->rom_alloc(size, GENERIC_ROM8_WIDTH, ENDIANNESS_LITTLE);
	m_cart->common_load_rom(m_cart->get_rom_base(), size, "rom");

	return image_init_result::PASS;
}

READ8_MEMBER(fidelbase_state::cartridge_r)
{
	if (m_cart->exists())
		return m_cart->read_rom(space, offset);
	else
		return 0;
}



// Offset-dependent CPU divider on some machines

void fidelbase_state::div_set_cpu_freq(offs_t offset)
{
	static const u16 mask = 0x6000;
	u16 status = (offset & mask) | m_div_config->read();

	if (status != m_div_status && status & 2)
	{
		// when a13/a14 is high, XTAL goes through divider(s)
		// (depending on factory-set jumper, either one or two 7474)
		float div = (status & 1) ? 0.25 : 0.5;
		m_maincpu->set_clock_scale((offset & mask) ? div : 1.0);
	}

	m_div_status = status;
}

void fidelbase_state::div_trampoline_w(offs_t offset, u8 data)
{
	div_set_cpu_freq(offset);
	m_mainmap->write8(offset, data);
}

u8 fidelbase_state::div_trampoline_r(offs_t offset)
{
	if (!machine().side_effects_disabled())
		div_set_cpu_freq(offset);

	return m_mainmap->read8(offset);
}

void fidelbase_state::div_trampoline(address_map &map)
{
	map(0x0000, 0xffff).rw(FUNC(fidelbase_state::div_trampoline_r), FUNC(fidelbase_state::div_trampoline_w));
}

INPUT_PORTS_START( fidel_cpu_div_2 )
	PORT_START("div_config") // hardwired, default to /2
	PORT_CONFNAME( 0x03, 0x02, "CPU Divider" )
	PORT_CONFSETTING(    0x00, "Disabled" )
	PORT_CONFSETTING(    0x02, "2" )
	PORT_CONFSETTING(    0x03, "4" )
INPUT_PORTS_END

INPUT_PORTS_START( fidel_cpu_div_4 )
	PORT_START("div_config") // hardwired, default to /4
	PORT_CONFNAME( 0x03, 0x03, "CPU Divider" )
	PORT_CONFSETTING(    0x00, "Disabled" )
	PORT_CONFSETTING(    0x02, "2" )
	PORT_CONFSETTING(    0x03, "4" )
INPUT_PORTS_END


// Devices, I/O

/******************************************************************************
    CCX, VCC/UVC
******************************************************************************/

// misc handlers

void vcc_state::prepare_display()
{
	// 4 7seg leds (note: sel d0 for extra leds)
	u8 outdata = (m_7seg_data & 0x7f) | (m_led_select << 7 & 0x80);
	set_display_segmask(0xf, 0x7f);
	display_matrix(8, 4, outdata, m_led_select >> 2 & 0xf);
}

READ8_MEMBER(vcc_state::speech_r)
{
	return m_speech_rom[m_speech_bank << 12 | offset];
}


// I8255 PPI

WRITE8_MEMBER(vcc_state::ppi_porta_w)
{
	// d0-d6: digit segment data, bits are xABCDEFG
	m_7seg_data = bitswap<8>(data,7,0,1,2,3,4,5,6);
	prepare_display();

	// d0-d5: TSI C0-C5
	// d7: TSI START line
	m_speech->data_w(space, 0, data & 0x3f);
	m_speech->start_w(data >> 7 & 1);

	// d6: language latch data
	// d7: language latch clock (latch on high)
	if (data & 0x80)
	{
		m_speech->force_update(); // update stream to now
		m_speech_bank = data >> 6 & 1;
	}
}

READ8_MEMBER(vcc_state::ppi_portb_r)
{
	// d7: TSI BUSY line
	return (m_speech->busy_r()) ? 0x80 : 0x00;
}

WRITE8_MEMBER(vcc_state::ppi_portb_w)
{
	// d0,d2-d5: digit/led select
	// _d6: enable language switches
	m_led_select = data;
	prepare_display();
}

READ8_MEMBER(vcc_state::ppi_portc_r)
{
	// d0-d3: multiplexed inputs (active low)
	// also language switches, hardwired with 4 jumpers
	// 0(none wired): English, 1: German, 2: French, 4: Spanish, 8:Special(unused)
	u8 lan = (~m_led_select & 0x40) ? m_language : 0;
	return ~(lan | read_inputs(4)) & 0xf;
}

WRITE8_MEMBER(vcc_state::ppi_portc_w)
{
	// d4-d7: input mux (inverted)
	m_inp_mux = ~data >> 4 & 0xf;
}


// CCX-specific (no speech chip, 1-bit beeper instead)

void ccx_state::prepare_display()
{
	// 4 7seg leds (note: sel d0 for extra leds)
	u8 outdata = (m_7seg_data & 0x7f) | (m_led_select << 7 & 0x80);
	set_display_segmask(0xf, 0x7f);
	display_matrix(8, 4, outdata, m_led_select >> 2 & 0xf);
}


// I8255 PPI

WRITE8_MEMBER(ccx_state::ppi_porta_w)
{
	// d7: enable beeper on falling edge (555 monostable)
	if (~data & m_7seg_data & 0x80 && !m_beeper_off->enabled())
	{
		m_beeper->set_state(1);
		m_beeper_off->adjust(attotime::from_msec(80)); // duration is approximate
	}

	// d0-d6: digit segment data
	m_7seg_data = bitswap<8>(data,7,0,1,2,3,4,5,6);
	prepare_display();
}

WRITE8_MEMBER(ccx_state::ppi_portb_w)
{
	// d0,d2-d5: digit/led select
	m_led_select = data;
	prepare_display();
}

READ8_MEMBER(ccx_state::ppi_portc_r)
{
	// d0-d3: multiplexed inputs (active low)
	return ~read_inputs(4) & 0xf;
}

WRITE8_MEMBER(ccx_state::ppi_portc_w)
{
	// d4-d7: input mux (inverted)
	m_inp_mux = ~data >> 4 & 0xf;
}



/******************************************************************************
    BCC, BKC
******************************************************************************/

// TTL

WRITE8_MEMBER(bcc_state::control_w)
{
	// a0-a2,d7: digit segment data via NE591
	u8 mask = 1 << (offset & 7);
	m_7seg_data = (m_7seg_data & ~mask) | ((data & 0x80) ? mask : 0);

	// BCC: NE591 Q7 is speaker out
	if (m_dac != nullptr)
		m_dac->write(BIT(m_7seg_data, 7));

	// d0-d3: led select, input mux
	// d4,d5: upper leds(direct)
	set_display_segmask(0xf, 0x7f);
	display_matrix(8, 6, m_7seg_data, data & 0x3f);
	m_inp_mux = data & 0xf;
}

READ8_MEMBER(bcc_state::input_r)
{
	// d0-d3: multiplexed inputs
	return read_inputs(4);
}



/******************************************************************************
    SCC
******************************************************************************/

// TTL

WRITE8_MEMBER(scc_state::control_w)
{
	// a0-a2,d7: led data
	u8 mask = 1 << (offset & 7);
	m_led_data = (m_led_data & ~mask) | ((data & 0x80) ? mask : 0);

	// d0-d3: led select, input mux (row 9 is speaker out)
	// d4: corner led(direct)
	m_inp_mux = 1 << (data & 0xf);
	m_dac->write(BIT(m_inp_mux, 9));
	display_matrix(8, 9, m_led_data, (m_inp_mux & 0xff) | (data << 4 & 0x100));
}

READ8_MEMBER(scc_state::input_r)
{
	// d0-d7: multiplexed inputs (active low)
	return ~read_inputs(9);
}



/******************************************************************************
    VSC
******************************************************************************/

// misc handlers

void vsc_state::prepare_display()
{
	// 4 7seg leds+H, 8*8 chessboard leds
	set_display_segmask(0xf, 0x7f);
	display_matrix(16, 8, m_led_data << 8 | m_7seg_data, m_led_select);
}

READ8_MEMBER(vsc_state::speech_r)
{
	return m_speech_rom[m_speech_bank << 12 | offset];
}


// I8255 PPI

WRITE8_MEMBER(vsc_state::ppi_porta_w)
{
	// d0-d5: TSI C0-C5
	m_speech->data_w(space, 0, data & 0x3f);

	// d0-d7: data for the 4 7seg leds, bits are HGCBAFED (H is extra led)
	m_7seg_data = bitswap<8>(data,7,6,2,1,0,5,4,3);
	prepare_display();
}

WRITE8_MEMBER(vsc_state::ppi_portb_w)
{
	// d0-d7: led row data
	m_led_data = data;
	prepare_display();
}

WRITE8_MEMBER(vsc_state::ppi_portc_w)
{
	// d0-d3: select digits
	// d0-d7: select leds, input mux low bits
	m_inp_mux = (m_inp_mux & ~0xff) | data;
	m_led_select = data;
	prepare_display();
}


// Z80 PIO

READ8_MEMBER(vsc_state::pio_porta_r)
{
	// d0-d7: multiplexed inputs
	// also language switches(hardwired with 2 diodes)
	u8 lan = (m_inp_mux & 0x400) ? m_language : 0;
	return read_inputs(10) | lan;
}

READ8_MEMBER(vsc_state::pio_portb_r)
{
	u8 data = 0;

	// d4: TSI BUSY line
	data |= (m_speech->busy_r()) ? 0 : 0x10;

	return data;
}

WRITE8_MEMBER(vsc_state::pio_portb_w)
{
	// d0,d1: input mux highest bits
	// d5: enable language switch
	m_inp_mux = (m_inp_mux & 0xff) | (data << 8 & 0x300) | (data << 5 & 0x400);

	// d7: TSI ROM A12
	m_speech->force_update(); // update stream to now
	m_speech_bank = data >> 7 & 1;

	// d6: TSI START line
	m_speech->start_w(data >> 6 & 1);

	// d2: lower TSI volume
	m_speech->set_output_gain(0, (data & 4) ? 0.5 : 1.0);
}



/******************************************************************************
    VBRC/UBC, BV3
******************************************************************************/

// misc handlers

void card_state::prepare_display()
{
	// 14seg led segments, d15(12) is extra led
	u16 outdata = bitswap<16>(m_7seg_data,12,13,1,6,5,2,0,7,15,11,10,14,4,3,9,8);
	set_display_segmask(0xff, 0x3fff);
	display_matrix(16, 8, outdata, m_led_select);
}

WRITE8_MEMBER(card_state::speech_w)
{
	if (m_speech == nullptr)
		return;

	m_speech->data_w(space, 0, data & 0x3f);
	m_speech->start_w(1);
	m_speech->start_w(0);
}


// I8243 I/O expander

template<int P>
void card_state::ioexp_port_w(uint8_t data)
{
	// P4x-P7x: digit segment data
	m_7seg_data = (m_7seg_data & ~(0xf << (4*P))) | ((data & 0xf) << (4*P));
	prepare_display();

	// P71 is tone (not on speech model)
	if (P == 3 && m_dac != nullptr)
		m_dac->write(BIT(data, 1));
}


// I8041 MCU

WRITE8_MEMBER(card_state::mcu_p1_w)
{
	// P10-P17: select digits, input mux
	m_inp_mux = m_led_select = data;
	prepare_display();
}

READ8_MEMBER(card_state::mcu_p2_r)
{
	// P20-P23: I8243 P2
	// P24-P27: multiplexed inputs (active low)
	return (m_i8243->p2_r() & 0x0f) | (read_inputs(8) << 4 ^ 0xf0);
}



/******************************************************************************
    DSC
******************************************************************************/

// TTL

void dsc_state::prepare_display()
{
	// 4 7seg leds
	set_display_segmask(0xf, 0x7f);
	display_matrix(8, 4, m_7seg_data, m_led_select);
}

WRITE8_MEMBER(dsc_state::control_w)
{
	// d0-d7: input mux, 7seg data
	m_inp_mux = ~data;
	m_7seg_data = data;
	prepare_display();
}

WRITE8_MEMBER(dsc_state::select_w)
{
	// d4: speaker out
	m_dac->write(BIT(~data, 4));

	// d0-d3: digit select
	m_led_select = data & 0xf;
	prepare_display();
}

READ8_MEMBER(dsc_state::input_r)
{
	// d0-d7: multiplexed inputs (active low)
	return ~read_inputs(8);
}



/******************************************************************************
    Address Maps
******************************************************************************/

// CCX, VCC/UVC

void ccx_state::main_map(address_map &map)
{
	map.unmap_value_high();
	map.global_mask(0x3fff);
	map(0x0000, 0x0fff).rom();
	map(0x1000, 0x10ff).mirror(0x0f00).ram();
	map(0x3000, 0x30ff).mirror(0x0f00).ram();
}

void ccx_state::main_io(address_map &map)
{
	map.global_mask(0x03);
	map(0x00, 0x03).rw(m_ppi8255, FUNC(i8255_device::read), FUNC(i8255_device::write));
}

void vcc_state::main_map(address_map &map)
{
	map.unmap_value_high();
	map(0x0000, 0x2fff).rom();
	map(0x4000, 0x43ff).mirror(0x1c00).ram();
}

void vcc_state::main_io(address_map &map)
{
	map.global_mask(0x03);
	map(0x00, 0x03).rw(m_ppi8255, FUNC(i8255_device::read), FUNC(i8255_device::write));
}


// BCC, BKC

void bcc_state::main_map(address_map &map)
{
	map.unmap_value_high();
	map.global_mask(0x3fff);
	map(0x0000, 0x0fff).rom();
	map(0x3000, 0x30ff).mirror(0x0f00).ram();
}

void bcc_state::main_io(address_map &map)
{
	map.global_mask(0x07);
	map(0x00, 0x07).rw(FUNC(bcc_state::input_r), FUNC(bcc_state::control_w));
}


// SCC

void scc_state::main_map(address_map &map)
{
	map(0x0000, 0x0fff).rom();
	map(0x5000, 0x50ff).ram();
}

void scc_state::main_io(address_map &map)
{
	map.global_mask(0x07);
	map(0x00, 0x07).rw(FUNC(scc_state::input_r), FUNC(scc_state::control_w));
}


// VSC

void vsc_state::main_map(address_map &map)
{
	map.unmap_value_high();
	map(0x0000, 0x3fff).rom();
	map(0x4000, 0x4fff).mirror(0x1000).rom();
	map(0x6000, 0x63ff).mirror(0x1c00).ram();
}

// VSC io: A2 is 8255 _CE, A3 is Z80 PIO _CE - in theory, both chips can be accessed simultaneously
READ8_MEMBER(vsc_state::main_io_trampoline_r)
{
	u8 data = 0xff; // open bus
	if (~offset & 4)
		data &= m_ppi8255->read(offset & 3);
	if (~offset & 8)
		data &= m_z80pio->read(space, offset & 3);

	return data;
}

WRITE8_MEMBER(vsc_state::main_io_trampoline_w)
{
	if (~offset & 4)
		m_ppi8255->write(offset & 3, data);
	if (~offset & 8)
		m_z80pio->write(space, offset & 3, data);
}

void vsc_state::main_io(address_map &map)
{
	map.global_mask(0x0f);
	map(0x00, 0x0f).rw(FUNC(vsc_state::main_io_trampoline_r), FUNC(vsc_state::main_io_trampoline_w));
}


// VBRC/UBC, BV3

void card_state::main_map(address_map &map)
{
	map.unmap_value_high();
	map(0x0000, 0x5fff).rom();
	map(0x6000, 0x63ff).mirror(0x1c00).ram();
	map(0xe000, 0xe000).mirror(0x1fff).w(FUNC(card_state::speech_w));
}

void card_state::main_io(address_map &map)
{
	map.global_mask(0x01);
	map(0x00, 0x01).rw(m_mcu, FUNC(i8041_device::upi41_master_r), FUNC(i8041_device::upi41_master_w));
}


// DSC

void dsc_state::main_map(address_map &map)
{
	map.unmap_value_high();
	map(0x0000, 0x1fff).rom();
	map(0x4000, 0x4000).mirror(0x1fff).w(FUNC(dsc_state::control_w));
	map(0x6000, 0x6000).mirror(0x1fff).w(FUNC(dsc_state::select_w));
	map(0x8000, 0x8000).mirror(0x1fff).r(FUNC(dsc_state::input_r));
	map(0xa000, 0xa3ff).mirror(0x1c00).ram();
}



/******************************************************************************
    Input Ports
******************************************************************************/

// static or boardless games

static INPUT_PORTS_START( vcc )
	PORT_START("IN.0")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("LV") PORT_CODE(KEYCODE_L)
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("A1") PORT_CODE(KEYCODE_1) PORT_CODE(KEYCODE_1_PAD) PORT_CODE(KEYCODE_A)
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("E5") PORT_CODE(KEYCODE_5) PORT_CODE(KEYCODE_5_PAD) PORT_CODE(KEYCODE_E)

	PORT_START("IN.1")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("Speaker") PORT_CODE(KEYCODE_SPACE)
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("DM") PORT_CODE(KEYCODE_M)
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("B2") PORT_CODE(KEYCODE_2) PORT_CODE(KEYCODE_2_PAD) PORT_CODE(KEYCODE_B)
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("F6") PORT_CODE(KEYCODE_6) PORT_CODE(KEYCODE_6_PAD) PORT_CODE(KEYCODE_F)

	PORT_START("IN.2")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("CL") PORT_CODE(KEYCODE_DEL)
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("PB") PORT_CODE(KEYCODE_P)
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("C3") PORT_CODE(KEYCODE_3) PORT_CODE(KEYCODE_3_PAD) PORT_CODE(KEYCODE_C)
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("G7") PORT_CODE(KEYCODE_7) PORT_CODE(KEYCODE_7_PAD) PORT_CODE(KEYCODE_G)

	PORT_START("IN.3")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("EN") PORT_CODE(KEYCODE_ENTER)
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("PV") PORT_CODE(KEYCODE_V)
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("D4") PORT_CODE(KEYCODE_4) PORT_CODE(KEYCODE_4_PAD) PORT_CODE(KEYCODE_D)
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("H8") PORT_CODE(KEYCODE_8) PORT_CODE(KEYCODE_8_PAD) PORT_CODE(KEYCODE_H)

	PORT_START("RESET") // is not on matrix IN.0 d0
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("RE") PORT_CODE(KEYCODE_R) PORT_CHANGED_MEMBER(DEVICE_SELF, vcc_state, reset_button, nullptr)
INPUT_PORTS_END

static INPUT_PORTS_START( ccx )
	PORT_START("IN.0")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("LV") PORT_CODE(KEYCODE_L)
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("A1") PORT_CODE(KEYCODE_1) PORT_CODE(KEYCODE_1_PAD) PORT_CODE(KEYCODE_A)
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("E5") PORT_CODE(KEYCODE_5) PORT_CODE(KEYCODE_5_PAD) PORT_CODE(KEYCODE_E)

	PORT_START("IN.1")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("Speaker") PORT_CODE(KEYCODE_SPACE)
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("DM") PORT_CODE(KEYCODE_M)
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("B2") PORT_CODE(KEYCODE_2) PORT_CODE(KEYCODE_2_PAD) PORT_CODE(KEYCODE_B)
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("F6") PORT_CODE(KEYCODE_6) PORT_CODE(KEYCODE_6_PAD) PORT_CODE(KEYCODE_F)

	PORT_START("IN.2")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("CL") PORT_CODE(KEYCODE_DEL)
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("PB") PORT_CODE(KEYCODE_P)
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("C3") PORT_CODE(KEYCODE_3) PORT_CODE(KEYCODE_3_PAD) PORT_CODE(KEYCODE_C)
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("G7") PORT_CODE(KEYCODE_7) PORT_CODE(KEYCODE_7_PAD) PORT_CODE(KEYCODE_G)

	PORT_START("IN.3")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("EN") PORT_CODE(KEYCODE_ENTER)
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("PV") PORT_CODE(KEYCODE_V)
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("D4") PORT_CODE(KEYCODE_4) PORT_CODE(KEYCODE_4_PAD) PORT_CODE(KEYCODE_D)
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("H8") PORT_CODE(KEYCODE_8) PORT_CODE(KEYCODE_8_PAD) PORT_CODE(KEYCODE_H)

	PORT_START("RESET") // is not on matrix IN.0 d0
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("RE") PORT_CODE(KEYCODE_R) PORT_CHANGED_MEMBER(DEVICE_SELF, ccx_state, reset_button, nullptr)

	PORT_START("LEVEL") // hardwired (VCC/GND?)
	PORT_CONFNAME( 0x80, 0x00, "Maximum Levels" )
	PORT_CONFSETTING(    0x00, "10" ) // factory setting
	PORT_CONFSETTING(    0x80, "3" )
INPUT_PORTS_END


static INPUT_PORTS_START( bcc )
	PORT_START("IN.0")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("EN") PORT_CODE(KEYCODE_ENTER)
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("PV") PORT_CODE(KEYCODE_V)
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("D4") PORT_CODE(KEYCODE_4) PORT_CODE(KEYCODE_4_PAD) PORT_CODE(KEYCODE_D)
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("H8") PORT_CODE(KEYCODE_8) PORT_CODE(KEYCODE_8_PAD) PORT_CODE(KEYCODE_H)

	PORT_START("IN.1")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("CL") PORT_CODE(KEYCODE_DEL)
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("PB") PORT_CODE(KEYCODE_P)
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("C3") PORT_CODE(KEYCODE_3) PORT_CODE(KEYCODE_3_PAD) PORT_CODE(KEYCODE_C)
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("G7") PORT_CODE(KEYCODE_7) PORT_CODE(KEYCODE_7_PAD) PORT_CODE(KEYCODE_G)

	PORT_START("IN.2")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("CB") PORT_CODE(KEYCODE_SPACE)
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("DM") PORT_CODE(KEYCODE_M)
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("B2") PORT_CODE(KEYCODE_2) PORT_CODE(KEYCODE_2_PAD) PORT_CODE(KEYCODE_B)
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("F6") PORT_CODE(KEYCODE_6) PORT_CODE(KEYCODE_6_PAD) PORT_CODE(KEYCODE_F)

	PORT_START("IN.3")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("RE") PORT_CODE(KEYCODE_R)
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("LV") PORT_CODE(KEYCODE_L)
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("A1") PORT_CODE(KEYCODE_1) PORT_CODE(KEYCODE_1_PAD) PORT_CODE(KEYCODE_A)
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("E5") PORT_CODE(KEYCODE_5) PORT_CODE(KEYCODE_5_PAD) PORT_CODE(KEYCODE_E)
INPUT_PORTS_END


static INPUT_PORTS_START( bkc )
	PORT_START("IN.0")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("EN") PORT_CODE(KEYCODE_ENTER)
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("9") PORT_CODE(KEYCODE_9) PORT_CODE(KEYCODE_9_PAD)
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("6") PORT_CODE(KEYCODE_6) PORT_CODE(KEYCODE_6_PAD)
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("3") PORT_CODE(KEYCODE_3) PORT_CODE(KEYCODE_3_PAD)

	PORT_START("IN.1")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("CL") PORT_CODE(KEYCODE_DEL)
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("8") PORT_CODE(KEYCODE_8) PORT_CODE(KEYCODE_8_PAD)
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("5") PORT_CODE(KEYCODE_5) PORT_CODE(KEYCODE_5_PAD)
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("2") PORT_CODE(KEYCODE_2) PORT_CODE(KEYCODE_2_PAD)

	PORT_START("IN.2")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("GM") PORT_CODE(KEYCODE_SPACE)
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("7") PORT_CODE(KEYCODE_7) PORT_CODE(KEYCODE_7_PAD)
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("4") PORT_CODE(KEYCODE_4) PORT_CODE(KEYCODE_4_PAD)
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("1") PORT_CODE(KEYCODE_1) PORT_CODE(KEYCODE_1_PAD)

	PORT_START("IN.3")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("RE") PORT_CODE(KEYCODE_R)
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("PB") PORT_CODE(KEYCODE_P)
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("PV") PORT_CODE(KEYCODE_V)
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("0") PORT_CODE(KEYCODE_0) PORT_CODE(KEYCODE_0_PAD)
INPUT_PORTS_END

INPUT_CHANGED_MEMBER(card_state::reset_button)
{
	// reset button is directly wired to maincpu/mcu RESET pins
	m_maincpu->set_input_line(INPUT_LINE_RESET, newval ? ASSERT_LINE : CLEAR_LINE);
	m_mcu->set_input_line(INPUT_LINE_RESET, newval ? ASSERT_LINE : CLEAR_LINE);
}

static INPUT_PORTS_START( vbrc )
	PORT_START("IN.0")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_A) PORT_NAME("A")
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_0) PORT_NAME("10")
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_6) PORT_NAME("6")
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_2) PORT_NAME("2")

	PORT_START("IN.1")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_K) PORT_NAME("K")
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_9) PORT_NAME("9")
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_5) PORT_NAME("5")
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_1) PORT_NAME("1")

	PORT_START("IN.2")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_Q) PORT_NAME("Q")
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_8) PORT_NAME("8")
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_4) PORT_NAME("4")
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_Z) PORT_NAME("P")

	PORT_START("IN.3")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_J) PORT_NAME("J")
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_7) PORT_NAME("7")
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_3) PORT_NAME("3")
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_N) PORT_NAME("NT")

	PORT_START("IN.4")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_E) PORT_NAME("EN")
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_S) PORT_NAME("SC")
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_X) PORT_NAME("PL")
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_1_PAD) PORT_NAME("Spades")

	PORT_START("IN.5")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_C) PORT_NAME("CL")
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_D) PORT_NAME("DB")
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_V) PORT_NAME("VL")
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_2_PAD) PORT_NAME("Hearts")

	PORT_START("IN.6")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_SPACE) PORT_NAME("Speaker")
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_B) PORT_NAME("PB")
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_G) PORT_NAME("CV")
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_3_PAD) PORT_NAME("Diamonds")

	PORT_START("IN.7")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_T) PORT_NAME("BR")
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_L) PORT_NAME("DL")
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_4_PAD) PORT_NAME("Clubs")

	PORT_START("RESET") // is not on matrix IN.7 d0
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_R) PORT_CHANGED_MEMBER(DEVICE_SELF, card_state, reset_button, nullptr) PORT_NAME("RE")

	PORT_START("BARCODE")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_OTHER) PORT_CODE(KEYCODE_F1) PORT_NAME("Card Scanner")
INPUT_PORTS_END


static INPUT_PORTS_START( bv3 )
	PORT_START("IN.0")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_A) PORT_NAME("Ace")
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_0) PORT_NAME("10")
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_6) PORT_NAME("6")
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_2) PORT_NAME("2")

	PORT_START("IN.1")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_K) PORT_NAME("King")
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_9) PORT_NAME("9")
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_5) PORT_NAME("5")
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_1) PORT_NAME("1")

	PORT_START("IN.2")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_Q) PORT_NAME("Queen")
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_8) PORT_NAME("8")
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_4) PORT_NAME("4")
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_Z) PORT_NAME("Quit")

	PORT_START("IN.3")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_J) PORT_NAME("Jack")
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_7) PORT_NAME("7")
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_3) PORT_NAME("3")
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_N) PORT_NAME("No Trump")

	PORT_START("IN.4")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_E) PORT_NAME("Yes/Enter")
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_S) PORT_NAME("No/Pass")
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_X) PORT_NAME("Player")
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_1_PAD) PORT_NAME("Spades")

	PORT_START("IN.5")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_C) PORT_NAME("Clear")
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_D) PORT_NAME("Double")
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_V) PORT_NAME("Score")
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_2_PAD) PORT_NAME("Hearts")

	PORT_START("IN.6")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_SPACE) PORT_NAME("Speaker")
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_B) PORT_NAME("Auto")
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_G) PORT_NAME("Conv")
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_3_PAD) PORT_NAME("Diamonds")

	PORT_START("IN.7")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_T) PORT_NAME("Review")
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_L) PORT_NAME("Dealer")
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_4_PAD) PORT_NAME("Clubs")

	PORT_START("RESET") // is not on matrix IN.7 d0
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_R) PORT_CHANGED_MEMBER(DEVICE_SELF, card_state, reset_button, nullptr) PORT_NAME("Reset")

	PORT_START("BARCODE")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_OTHER) PORT_CODE(KEYCODE_F1) PORT_NAME("Card Scanner")
INPUT_PORTS_END


// sensory board games

INPUT_PORTS_START( fidel_cb_buttons )
	PORT_START("IN.0")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("Board Sensor")
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("Board Sensor")
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("Board Sensor")
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("Board Sensor")
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("Board Sensor")
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("Board Sensor")
	PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("Board Sensor")
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("Board Sensor")

	PORT_START("IN.1")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("Board Sensor")
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("Board Sensor")
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("Board Sensor")
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("Board Sensor")
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("Board Sensor")
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("Board Sensor")
	PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("Board Sensor")
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("Board Sensor")

	PORT_START("IN.2")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("Board Sensor")
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("Board Sensor")
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("Board Sensor")
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("Board Sensor")
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("Board Sensor")
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("Board Sensor")
	PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("Board Sensor")
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("Board Sensor")

	PORT_START("IN.3")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("Board Sensor")
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("Board Sensor")
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("Board Sensor")
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("Board Sensor")
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("Board Sensor")
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("Board Sensor")
	PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("Board Sensor")
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("Board Sensor")

	PORT_START("IN.4")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("Board Sensor")
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("Board Sensor")
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("Board Sensor")
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("Board Sensor")
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("Board Sensor")
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("Board Sensor")
	PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("Board Sensor")
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("Board Sensor")

	PORT_START("IN.5")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("Board Sensor")
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("Board Sensor")
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("Board Sensor")
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("Board Sensor")
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("Board Sensor")
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("Board Sensor")
	PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("Board Sensor")
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("Board Sensor")

	PORT_START("IN.6")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("Board Sensor")
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("Board Sensor")
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("Board Sensor")
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("Board Sensor")
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("Board Sensor")
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("Board Sensor")
	PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("Board Sensor")
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("Board Sensor")

	PORT_START("IN.7")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("Board Sensor")
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("Board Sensor")
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("Board Sensor")
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("Board Sensor")
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("Board Sensor")
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("Board Sensor")
	PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("Board Sensor")
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("Board Sensor")
INPUT_PORTS_END

INPUT_PORTS_START( fidel_cb_magnets )
	PORT_START("IN.0")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_TOGGLE PORT_NAME("Board Sensor")
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_TOGGLE PORT_NAME("Board Sensor")
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_TOGGLE PORT_NAME("Board Sensor")
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_TOGGLE PORT_NAME("Board Sensor")
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_TOGGLE PORT_NAME("Board Sensor")
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_TOGGLE PORT_NAME("Board Sensor")
	PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_TOGGLE PORT_NAME("Board Sensor")
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_TOGGLE PORT_NAME("Board Sensor")

	PORT_START("IN.1")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_TOGGLE PORT_NAME("Board Sensor")
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_TOGGLE PORT_NAME("Board Sensor")
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_TOGGLE PORT_NAME("Board Sensor")
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_TOGGLE PORT_NAME("Board Sensor")
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_TOGGLE PORT_NAME("Board Sensor")
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_TOGGLE PORT_NAME("Board Sensor")
	PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_TOGGLE PORT_NAME("Board Sensor")
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_TOGGLE PORT_NAME("Board Sensor")

	PORT_START("IN.2")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_TOGGLE PORT_NAME("Board Sensor")
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_TOGGLE PORT_NAME("Board Sensor")
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_TOGGLE PORT_NAME("Board Sensor")
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_TOGGLE PORT_NAME("Board Sensor")
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_TOGGLE PORT_NAME("Board Sensor")
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_TOGGLE PORT_NAME("Board Sensor")
	PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_TOGGLE PORT_NAME("Board Sensor")
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_TOGGLE PORT_NAME("Board Sensor")

	PORT_START("IN.3")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_TOGGLE PORT_NAME("Board Sensor")
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_TOGGLE PORT_NAME("Board Sensor")
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_TOGGLE PORT_NAME("Board Sensor")
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_TOGGLE PORT_NAME("Board Sensor")
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_TOGGLE PORT_NAME("Board Sensor")
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_TOGGLE PORT_NAME("Board Sensor")
	PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_TOGGLE PORT_NAME("Board Sensor")
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_TOGGLE PORT_NAME("Board Sensor")

	PORT_START("IN.4")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_TOGGLE PORT_NAME("Board Sensor")
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_TOGGLE PORT_NAME("Board Sensor")
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_TOGGLE PORT_NAME("Board Sensor")
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_TOGGLE PORT_NAME("Board Sensor")
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_TOGGLE PORT_NAME("Board Sensor")
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_TOGGLE PORT_NAME("Board Sensor")
	PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_TOGGLE PORT_NAME("Board Sensor")
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_TOGGLE PORT_NAME("Board Sensor")

	PORT_START("IN.5")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_TOGGLE PORT_NAME("Board Sensor")
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_TOGGLE PORT_NAME("Board Sensor")
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_TOGGLE PORT_NAME("Board Sensor")
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_TOGGLE PORT_NAME("Board Sensor")
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_TOGGLE PORT_NAME("Board Sensor")
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_TOGGLE PORT_NAME("Board Sensor")
	PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_TOGGLE PORT_NAME("Board Sensor")
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_TOGGLE PORT_NAME("Board Sensor")

	PORT_START("IN.6")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_TOGGLE PORT_NAME("Board Sensor")
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_TOGGLE PORT_NAME("Board Sensor")
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_TOGGLE PORT_NAME("Board Sensor")
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_TOGGLE PORT_NAME("Board Sensor")
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_TOGGLE PORT_NAME("Board Sensor")
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_TOGGLE PORT_NAME("Board Sensor")
	PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_TOGGLE PORT_NAME("Board Sensor")
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_TOGGLE PORT_NAME("Board Sensor")

	PORT_START("IN.7")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_TOGGLE PORT_NAME("Board Sensor")
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_TOGGLE PORT_NAME("Board Sensor")
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_TOGGLE PORT_NAME("Board Sensor")
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_TOGGLE PORT_NAME("Board Sensor")
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_TOGGLE PORT_NAME("Board Sensor")
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_TOGGLE PORT_NAME("Board Sensor")
	PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_TOGGLE PORT_NAME("Board Sensor")
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_TOGGLE PORT_NAME("Board Sensor")
INPUT_PORTS_END


static INPUT_PORTS_START( scc )
	PORT_INCLUDE( fidel_cb_buttons )

	PORT_START("IN.8")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_1) PORT_CODE(KEYCODE_1_PAD) PORT_NAME("Pawn")
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_2) PORT_CODE(KEYCODE_2_PAD) PORT_NAME("Rook")
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_3) PORT_CODE(KEYCODE_3_PAD) PORT_NAME("Knight")
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_4) PORT_CODE(KEYCODE_4_PAD) PORT_NAME("Bishop")
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_5) PORT_CODE(KEYCODE_5_PAD) PORT_NAME("Queen")
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_6) PORT_CODE(KEYCODE_6_PAD) PORT_NAME("King")
	PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_DEL) PORT_NAME("CL")
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_R) PORT_NAME("RE")
INPUT_PORTS_END


static INPUT_PORTS_START( vsc )
	PORT_INCLUDE( scc )

	PORT_START("IN.9")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_T) PORT_NAME("TM")
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_V) PORT_NAME("RV")
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_SPACE) PORT_NAME("Speaker")
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_L) PORT_NAME("LV")
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_M) PORT_NAME("DM")
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_S) PORT_NAME("ST")
	PORT_BIT(0xc0, IP_ACTIVE_HIGH, IPT_UNUSED)
INPUT_PORTS_END


static INPUT_PORTS_START( dsc )
	PORT_INCLUDE( fidel_cb_buttons )

	PORT_MODIFY("IN.4")
	PORT_BIT(0x8f, IP_ACTIVE_HIGH, IPT_UNUSED)

	PORT_MODIFY("IN.6")
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_1) PORT_CODE(KEYCODE_1_PAD) PORT_NAME("Black King")

	PORT_MODIFY("IN.7")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_2) PORT_CODE(KEYCODE_2_PAD) PORT_NAME("Black")
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_3) PORT_CODE(KEYCODE_3_PAD) PORT_NAME("White King")
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_4) PORT_CODE(KEYCODE_4_PAD) PORT_NAME("White")
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_5) PORT_CODE(KEYCODE_5_PAD) PORT_NAME("RV")
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_9) PORT_CODE(KEYCODE_9_PAD) PORT_NAME("RE")
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_8) PORT_CODE(KEYCODE_8_PAD) PORT_NAME("PB")
	PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_7) PORT_CODE(KEYCODE_7_PAD) PORT_NAME("LV")
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_6) PORT_CODE(KEYCODE_6_PAD) PORT_NAME("CL")
INPUT_PORTS_END



/******************************************************************************
    Machine Drivers
******************************************************************************/

void bcc_state::bkc(machine_config &config)
{
	/* basic machine hardware */
	Z80(config, m_maincpu, 3.579545_MHz_XTAL);
	m_maincpu->set_addrmap(AS_PROGRAM, &bcc_state::main_map);
	m_maincpu->set_addrmap(AS_IO, &bcc_state::main_io);

	TIMER(config, "display_decay").configure_periodic(FUNC(fidelbase_state::display_decay_tick), attotime::from_msec(1));
	config.set_default_layout(layout_fidel_bkc);
}

void bcc_state::bcc(machine_config &config)
{
	bkc(config);
	config.set_default_layout(layout_fidel_bcc);

	/* sound hardware */
	SPEAKER(config, "speaker").front_center();
	DAC_1BIT(config, m_dac, 0).add_route(ALL_OUTPUTS, "speaker", 0.25);
	voltage_regulator_device &vref(VOLTAGE_REGULATOR(config, "vref", 0));
	vref.set_output(5.0);
	vref.add_route(0, "dac", 1.0, DAC_VREF_POS_INPUT);
}

void scc_state::scc(machine_config &config)
{
	/* basic machine hardware */
	Z80(config, m_maincpu, 3.9_MHz_XTAL);
	m_maincpu->set_addrmap(AS_PROGRAM, &scc_state::main_map);
	m_maincpu->set_addrmap(AS_IO, &scc_state::main_io);

	TIMER(config, "display_decay").configure_periodic(FUNC(fidelbase_state::display_decay_tick), attotime::from_msec(1));
	config.set_default_layout(layout_fidel_sc8);

	/* sound hardware */
	SPEAKER(config, "speaker").front_center();
	DAC_1BIT(config, m_dac, 0).add_route(ALL_OUTPUTS, "speaker", 0.25);
	voltage_regulator_device &vref(VOLTAGE_REGULATOR(config, "vref", 0));
	vref.set_output(5.0);
	vref.add_route(0, "dac", 1.0, DAC_VREF_POS_INPUT);
}

void ccx_state::ccx(machine_config &config)
{
	/* basic machine hardware */
	Z80(config, m_maincpu, 4_MHz_XTAL);
	m_maincpu->set_addrmap(AS_PROGRAM, &ccx_state::main_map);
	m_maincpu->set_addrmap(AS_IO, &ccx_state::main_io);

	I8255(config, m_ppi8255);
	m_ppi8255->out_pa_callback().set(FUNC(ccx_state::ppi_porta_w));
	m_ppi8255->tri_pa_callback().set_constant(0);
	m_ppi8255->in_pb_callback().set_ioport("LEVEL");
	m_ppi8255->out_pb_callback().set(FUNC(ccx_state::ppi_portb_w));
	m_ppi8255->in_pc_callback().set(FUNC(ccx_state::ppi_portc_r));
	m_ppi8255->tri_pb_callback().set_constant(0);
	m_ppi8255->out_pc_callback().set(FUNC(ccx_state::ppi_portc_w));

	TIMER(config, "display_decay").configure_periodic(FUNC(fidelbase_state::display_decay_tick), attotime::from_msec(1));
	config.set_default_layout(layout_fidel_cc10);

	/* sound hardware */
	SPEAKER(config, "speaker").front_center();
	BEEP(config, m_beeper, 1360); // approximation, from 556 timer ic
	m_beeper->add_route(ALL_OUTPUTS, "speaker", 0.25);
	TIMER(config, "beeper_off").configure_generic(FUNC(ccx_state::beeper_off));
}

void vcc_state::vcc(machine_config &config)
{
	/* basic machine hardware */
	Z80(config, m_maincpu, 4_MHz_XTAL);
	m_maincpu->set_addrmap(AS_PROGRAM, &vcc_state::main_map);
	m_maincpu->set_addrmap(AS_IO, &vcc_state::main_io);

	I8255(config, m_ppi8255);
	m_ppi8255->out_pa_callback().set(FUNC(vcc_state::ppi_porta_w));
	m_ppi8255->tri_pa_callback().set_constant(0);
	m_ppi8255->in_pb_callback().set(FUNC(vcc_state::ppi_portb_r));
	m_ppi8255->out_pb_callback().set(FUNC(vcc_state::ppi_portb_w));
	m_ppi8255->tri_pb_callback().set_constant(0);
	m_ppi8255->in_pc_callback().set(FUNC(vcc_state::ppi_portc_r));
	m_ppi8255->out_pc_callback().set(FUNC(vcc_state::ppi_portc_w));

	TIMER(config, "display_decay").configure_periodic(FUNC(fidelbase_state::display_decay_tick), attotime::from_msec(1));
	config.set_default_layout(layout_fidel_vcc);

	/* sound hardware */
	SPEAKER(config, "speaker").front_center();
	S14001A(config, m_speech, 25000); // R/C circuit, around 25khz
	m_speech->ext_read().set(FUNC(vcc_state::speech_r));
	m_speech->add_route(ALL_OUTPUTS, "speaker", 0.75);
}

void vsc_state::vsc(machine_config &config)
{
	/* basic machine hardware */
	Z80(config, m_maincpu, 3.9_MHz_XTAL); // 3.9MHz resonator
	m_maincpu->set_addrmap(AS_PROGRAM, &vsc_state::main_map);
	m_maincpu->set_addrmap(AS_IO, &vsc_state::main_io);

	const attotime irq_period = attotime::from_hz(587); // 555 timer, measured
	TIMER(config, m_irq_on).configure_periodic(FUNC(vsc_state::irq_on<INPUT_LINE_NMI>), irq_period);
	m_irq_on->set_start_delay(irq_period - attotime::from_usec(845)); // active for 0.845ms (approx half)
	TIMER(config, "irq_off").configure_periodic(FUNC(vsc_state::irq_off<INPUT_LINE_NMI>), irq_period);

	I8255(config, m_ppi8255);
	m_ppi8255->out_pa_callback().set(FUNC(vsc_state::ppi_porta_w));
	m_ppi8255->out_pb_callback().set(FUNC(vsc_state::ppi_portb_w));
	m_ppi8255->out_pc_callback().set(FUNC(vsc_state::ppi_portc_w));

	Z80PIO(config, m_z80pio, 3.9_MHz_XTAL);
	m_z80pio->in_pa_callback().set(FUNC(vsc_state::pio_porta_r));
	m_z80pio->in_pb_callback().set(FUNC(vsc_state::pio_portb_r));
	m_z80pio->out_pb_callback().set(FUNC(vsc_state::pio_portb_w));

	TIMER(config, "display_decay").configure_periodic(FUNC(fidelbase_state::display_decay_tick), attotime::from_msec(1));
	config.set_default_layout(layout_fidel_vsc);

	/* sound hardware */
	SPEAKER(config, "speaker").front_center();
	S14001A(config, m_speech, 25000); // R/C circuit, around 25khz
	m_speech->ext_read().set(FUNC(vsc_state::speech_r));
	m_speech->add_route(ALL_OUTPUTS, "speaker", 0.75);
}

void card_state::brc_base(machine_config &config)
{
	/* basic machine hardware */
	Z80(config, m_maincpu, 5_MHz_XTAL/2);
	m_maincpu->set_addrmap(AS_PROGRAM, &card_state::main_map);
	m_maincpu->set_addrmap(AS_IO, &card_state::main_io);
	config.m_perfect_cpu_quantum = subtag("maincpu");

	I8041(config, m_mcu, 5_MHz_XTAL);
	m_mcu->p1_out_cb().set(FUNC(card_state::mcu_p1_w));
	m_mcu->p2_in_cb().set(FUNC(card_state::mcu_p2_r));
	m_mcu->p2_out_cb().set(m_i8243, FUNC(i8243_device::p2_w));
	m_mcu->prog_out_cb().set(m_i8243, FUNC(i8243_device::prog_w));
	m_mcu->t0_in_cb().set_ioport("BARCODE"); // card scanner

	// MCU T1 tied to master clock / 4
	CLOCK(config, "t1_clock", 5_MHz_XTAL/4).signal_handler().set_nop();
	m_mcu->t1_in_cb().set("t1_clock", FUNC(clock_device::signal_r)).invert();

	I8243(config, m_i8243);
	m_i8243->p4_out_cb().set(FUNC(card_state::ioexp_port_w<0>));
	m_i8243->p5_out_cb().set(FUNC(card_state::ioexp_port_w<1>));
	m_i8243->p6_out_cb().set(FUNC(card_state::ioexp_port_w<2>));
	m_i8243->p7_out_cb().set(FUNC(card_state::ioexp_port_w<3>));

	TIMER(config, "display_decay").configure_periodic(FUNC(fidelbase_state::display_decay_tick), attotime::from_msec(1));
	config.set_default_layout(layout_fidel_vbrc);
}

void card_state::ubc(machine_config &config)
{
	brc_base(config);

	/* sound hardware */
	SPEAKER(config, "speaker").front_center();
	DAC_1BIT(config, m_dac, 0).add_route(ALL_OUTPUTS, "speaker", 0.25);
	voltage_regulator_device &vref(VOLTAGE_REGULATOR(config, "vref", 0));
	vref.set_output(5.0);
	vref.add_route(0, "dac", 1.0, DAC_VREF_POS_INPUT);
}

void card_state::vbrc(machine_config &config)
{
	brc_base(config);

	/* sound hardware */
	SPEAKER(config, "speaker").front_center();
	S14001A(config, m_speech, 25000); // R/C circuit, around 25khz
	m_speech->bsy().set_inputline("maincpu", Z80_INPUT_LINE_WAIT);
	m_speech->add_route(ALL_OUTPUTS, "speaker", 0.75);
}

void card_state::bv3(machine_config &config)
{
	vbrc(config);
	config.set_default_layout(layout_fidel_bv3);
}

void dsc_state::dsc(machine_config &config)
{
	/* basic machine hardware */
	Z80(config, m_maincpu, 3.9_MHz_XTAL); // 3.9MHz resonator
	m_maincpu->set_addrmap(AS_PROGRAM, &dsc_state::main_map);

	const attotime irq_period = attotime::from_hz(523); // from 555 timer (22nF, 120K, 2.7K)
	TIMER(config, m_irq_on).configure_periodic(FUNC(dsc_state::irq_on<INPUT_LINE_IRQ0>), irq_period);
	m_irq_on->set_start_delay(irq_period - attotime::from_usec(41)); // active for 41us
	TIMER(config, "irq_off").configure_periodic(FUNC(dsc_state::irq_off<INPUT_LINE_IRQ0>), irq_period);

	TIMER(config, "display_decay").configure_periodic(FUNC(fidelbase_state::display_decay_tick), attotime::from_msec(1));
	config.set_default_layout(layout_fidel_dsc);

	/* sound hardware */
	SPEAKER(config, "speaker").front_center();
	DAC_1BIT(config, m_dac, 0).add_route(ALL_OUTPUTS, "speaker", 0.25);
	voltage_regulator_device &vref(VOLTAGE_REGULATOR(config, "vref", 0));
	vref.set_output(5.0);
	vref.add_route(0, "dac", 1.0, DAC_VREF_POS_INPUT);
}



/******************************************************************************
    ROM Definitions
******************************************************************************/

ROM_START( cc10 ) // model CCX
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "cn19053n_cc10b", 0x0000, 0x1000, CRC(afd3ca99) SHA1(870d09b2b52ccb8572d69642c59b5215d5fb26ab) ) // 2332
ROM_END


ROM_START( cc7 ) // model BCC
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "cn19103n_bcc-revb", 0x0000, 0x1000, CRC(a397d471) SHA1(9b12bc442fccee40f4d8500c792bc9d886c5e1a5) ) // 2332
ROM_END


ROM_START( backgamc ) // model BKC, PCB label P-380A-5
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "cn19255n_101-32012", 0x0000, 0x1000, CRC(0a8a19b7) SHA1(d6f0dd44b33c9b79570cf0ceac02a036ec91ba57) ) // 2332
ROM_END


ROM_START( fscc8 ) // model SCC, PCB label 510-1011 REV.2
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "101-32017", 0x0000, 0x1000, CRC(5340820d) SHA1(e3494c7624b3cacbbb9a0a8cc9e1ed3e00326dfd) ) // 2732
ROM_END


ROM_START( vcc )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD("cn19256n_101-32013", 0x0000, 0x1000, CRC(257bb5ab) SHA1(f7589225bb8e5f3eac55f23e2bd526be780b38b5) )
	ROM_LOAD("cn19174n_vcc2", 0x1000, 0x1000, CRC(f33095e7) SHA1(692fcab1b88c910b74d04fe4d0660367aee3f4f0) )
	ROM_LOAD("cn19175n_vcc3", 0x2000, 0x1000, CRC(624f0cd5) SHA1(7c1a4f4497fe5882904de1d6fecf510c07ee6fc6) )

	ROM_REGION( 0x2000, "speech", 0 )
	ROM_LOAD("101-32107", 0x0000, 0x1000, CRC(f35784f9) SHA1(348e54a7fa1e8091f89ac656b4da22f28ca2e44d) )
	ROM_RELOAD(           0x1000, 0x1000)
ROM_END

ROM_START( vccg )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD("cn19256n_101-32013", 0x0000, 0x1000, CRC(257bb5ab) SHA1(f7589225bb8e5f3eac55f23e2bd526be780b38b5) )
	ROM_LOAD("cn19174n_vcc2", 0x1000, 0x1000, CRC(f33095e7) SHA1(692fcab1b88c910b74d04fe4d0660367aee3f4f0) )
	ROM_LOAD("cn19175n_vcc3", 0x2000, 0x1000, CRC(624f0cd5) SHA1(7c1a4f4497fe5882904de1d6fecf510c07ee6fc6) )

	ROM_REGION( 0x2000, "speech", 0 )
	ROM_LOAD("101-64101", 0x0000, 0x2000, BAD_DUMP CRC(6c85e310) SHA1(20d1d6543c1e6a1f04184a2df2a468f33faec3ff) ) // taken from fexcelv, assume correct
ROM_END

ROM_START( vccsp )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD("cn19256n_101-32013", 0x0000, 0x1000, CRC(257bb5ab) SHA1(f7589225bb8e5f3eac55f23e2bd526be780b38b5) )
	ROM_LOAD("cn19174n_vcc2", 0x1000, 0x1000, CRC(f33095e7) SHA1(692fcab1b88c910b74d04fe4d0660367aee3f4f0) )
	ROM_LOAD("cn19175n_vcc3", 0x2000, 0x1000, CRC(624f0cd5) SHA1(7c1a4f4497fe5882904de1d6fecf510c07ee6fc6) )

	ROM_REGION( 0x2000, "speech", 0 )
	ROM_LOAD("101-64106", 0x0000, 0x2000, CRC(8766e128) SHA1(78c7413bf240159720b131ab70bfbdf4e86eb1e9) ) // dumped from Spanish VCC, is same as data in fexcelv
ROM_END

ROM_START( vccfr )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD("cn19256n_101-32013", 0x0000, 0x1000, CRC(257bb5ab) SHA1(f7589225bb8e5f3eac55f23e2bd526be780b38b5) )
	ROM_LOAD("cn19174n_vcc2", 0x1000, 0x1000, CRC(f33095e7) SHA1(692fcab1b88c910b74d04fe4d0660367aee3f4f0) )
	ROM_LOAD("cn19175n_vcc3", 0x2000, 0x1000, CRC(624f0cd5) SHA1(7c1a4f4497fe5882904de1d6fecf510c07ee6fc6) )

	ROM_REGION( 0x2000, "speech", 0 )
	ROM_LOAD("101-64105", 0x0000, 0x2000, BAD_DUMP CRC(fe8c5c18) SHA1(2b64279ab3747ee81c86963c13e78321c6cfa3a3) ) // taken from fexcelv, assume correct
ROM_END


ROM_START( uvc )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD("101-64017", 0x0000, 0x2000, CRC(f1133abf) SHA1(09dd85051c4e7d364d43507c1cfea5c2d08d37f4) ) // MOS // 101-64017 // 3880
	ROM_LOAD("101-32010", 0x2000, 0x1000, CRC(624f0cd5) SHA1(7c1a4f4497fe5882904de1d6fecf510c07ee6fc6) ) // NEC P9Z021 // D2332C 228 // 101-32010, == cn19175n_vcc3 on vcc

	ROM_REGION( 0x2000, "speech", 0 )
	ROM_LOAD("101-32107", 0x0000, 0x1000, CRC(f35784f9) SHA1(348e54a7fa1e8091f89ac656b4da22f28ca2e44d) ) // NEC P9Y019 // D2332C 229 // 101-32107
	ROM_RELOAD(           0x1000, 0x1000)
ROM_END

ROM_START( uvcg )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD("101-64017", 0x0000, 0x2000, CRC(f1133abf) SHA1(09dd85051c4e7d364d43507c1cfea5c2d08d37f4) )
	ROM_LOAD("101-32010", 0x2000, 0x1000, CRC(624f0cd5) SHA1(7c1a4f4497fe5882904de1d6fecf510c07ee6fc6) )

	ROM_REGION( 0x2000, "speech", 0 )
	ROM_LOAD("101-64101", 0x0000, 0x2000, BAD_DUMP CRC(6c85e310) SHA1(20d1d6543c1e6a1f04184a2df2a468f33faec3ff) ) // taken from fexcelv, assume correct
ROM_END

ROM_START( uvcsp )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD("101-64017", 0x0000, 0x2000, CRC(f1133abf) SHA1(09dd85051c4e7d364d43507c1cfea5c2d08d37f4) )
	ROM_LOAD("101-32010", 0x2000, 0x1000, CRC(624f0cd5) SHA1(7c1a4f4497fe5882904de1d6fecf510c07ee6fc6) )

	ROM_REGION( 0x2000, "speech", 0 )
	ROM_LOAD("101-64106", 0x0000, 0x2000, CRC(8766e128) SHA1(78c7413bf240159720b131ab70bfbdf4e86eb1e9) )
ROM_END

ROM_START( uvcfr )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD("101-64017", 0x0000, 0x2000, CRC(f1133abf) SHA1(09dd85051c4e7d364d43507c1cfea5c2d08d37f4) )
	ROM_LOAD("101-32010", 0x2000, 0x1000, CRC(624f0cd5) SHA1(7c1a4f4497fe5882904de1d6fecf510c07ee6fc6) )

	ROM_REGION( 0x2000, "speech", 0 )
	ROM_LOAD("101-64105", 0x0000, 0x2000, BAD_DUMP CRC(fe8c5c18) SHA1(2b64279ab3747ee81c86963c13e78321c6cfa3a3) ) // taken from fexcelv, assume correct
ROM_END


ROM_START( vsc )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD("101-64018", 0x0000, 0x2000, CRC(c9c98490) SHA1(e6db883df088d60463e75db51433a4b01a3e7626) )
	ROM_LOAD("101-64019", 0x2000, 0x2000, CRC(08a3577c) SHA1(69fe379d21a9d4b57c84c3832d7b3e7431eec341) )
	ROM_LOAD("101-32024", 0x4000, 0x1000, CRC(2a078676) SHA1(db2f0aba7e8ac0f84a17bae7155210cdf0813afb) )

	ROM_REGION( 0x2000, "speech", 0 )
	ROM_LOAD("101-32107", 0x0000, 0x1000, CRC(f35784f9) SHA1(348e54a7fa1e8091f89ac656b4da22f28ca2e44d) )
	ROM_RELOAD(           0x1000, 0x1000)
ROM_END

ROM_START( vscg )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD("101-64018", 0x0000, 0x2000, CRC(c9c98490) SHA1(e6db883df088d60463e75db51433a4b01a3e7626) )
	ROM_LOAD("101-64019", 0x2000, 0x2000, CRC(08a3577c) SHA1(69fe379d21a9d4b57c84c3832d7b3e7431eec341) )
	ROM_LOAD("101-32024", 0x4000, 0x1000, CRC(2a078676) SHA1(db2f0aba7e8ac0f84a17bae7155210cdf0813afb) )

	ROM_REGION( 0x2000, "speech", 0 )
	ROM_LOAD("101-64101", 0x0000, 0x2000, BAD_DUMP CRC(6c85e310) SHA1(20d1d6543c1e6a1f04184a2df2a468f33faec3ff) ) // taken from fexcelv, assume correct
ROM_END

ROM_START( vscsp )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD("101-64018", 0x0000, 0x2000, CRC(c9c98490) SHA1(e6db883df088d60463e75db51433a4b01a3e7626) )
	ROM_LOAD("101-64019", 0x2000, 0x2000, CRC(08a3577c) SHA1(69fe379d21a9d4b57c84c3832d7b3e7431eec341) )
	ROM_LOAD("101-32024", 0x4000, 0x1000, CRC(2a078676) SHA1(db2f0aba7e8ac0f84a17bae7155210cdf0813afb) )

	ROM_REGION( 0x2000, "speech", 0 )
	ROM_LOAD("101-64106", 0x0000, 0x2000, BAD_DUMP CRC(8766e128) SHA1(78c7413bf240159720b131ab70bfbdf4e86eb1e9) ) // taken from vcc/fexcelv, assume correct
ROM_END

ROM_START( vscfr )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD("101-64018", 0x0000, 0x2000, CRC(c9c98490) SHA1(e6db883df088d60463e75db51433a4b01a3e7626) )
	ROM_LOAD("101-64019", 0x2000, 0x2000, CRC(08a3577c) SHA1(69fe379d21a9d4b57c84c3832d7b3e7431eec341) )
	ROM_LOAD("101-32024", 0x4000, 0x1000, CRC(2a078676) SHA1(db2f0aba7e8ac0f84a17bae7155210cdf0813afb) )

	ROM_REGION( 0x2000, "speech", 0 )
	ROM_LOAD("101-64105", 0x0000, 0x2000, BAD_DUMP CRC(fe8c5c18) SHA1(2b64279ab3747ee81c86963c13e78321c6cfa3a3) ) // taken from fexcelv, assume correct
ROM_END


ROM_START( vbrc ) // model VBRC aka 7002
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD("101-64108", 0x0000, 0x2000, CRC(08472223) SHA1(859865b13c908dbb474333263dc60f6a32461141) ) // NEC 2364
	ROM_LOAD("101-64109", 0x2000, 0x2000, CRC(320afa0f) SHA1(90edfe0ac19b108d232cda376b03a3a24befad4c) ) // NEC 2364
	ROM_LOAD("101-64110", 0x4000, 0x2000, CRC(3040d0bd) SHA1(caa55fc8d9196e408fb41e7171a68e5099519813) ) // NEC 2364

	ROM_REGION( 0x0400, "mcu", 0 )
	ROM_LOAD("100-1009", 0x0000, 0x0400, CRC(60eb343f) SHA1(8a63e95ebd62e123bdecc330c0484a47c354bd1a) )

	ROM_REGION( 0x1000, "speech", 0 )
	ROM_LOAD("101-32118", 0x0000, 0x1000, CRC(a0b8bb8f) SHA1(f56852108928d5c6caccfc8166fa347d6760a740) )
ROM_END

ROM_START( bridgeca ) // model UBC
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD("101-64108", 0x0000, 0x2000, CRC(08472223) SHA1(859865b13c908dbb474333263dc60f6a32461141) )
	ROM_LOAD("101-64109", 0x2000, 0x2000, CRC(320afa0f) SHA1(90edfe0ac19b108d232cda376b03a3a24befad4c) )
	ROM_LOAD("101-64110", 0x4000, 0x2000, CRC(3040d0bd) SHA1(caa55fc8d9196e408fb41e7171a68e5099519813) )

	ROM_REGION( 0x0400, "mcu", 0 )
	ROM_LOAD("100-1009", 0x0000, 0x0400, CRC(60eb343f) SHA1(8a63e95ebd62e123bdecc330c0484a47c354bd1a) )
ROM_END


ROM_START( bridgec3 ) // model BV3 aka 7014, PCB label 510-1016 Rev.1
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD("7014_white", 0x0000, 0x2000, CRC(eb1620ef) SHA1(987a9abc8c685f1a68678ea4ee65ec4a99419179) ) // TMM2764AD-20, white sticker
	ROM_LOAD("7014_red",   0x2000, 0x2000, CRC(74af0019) SHA1(8dc05950c254ca050b95b93e5d0cf48f913a6d49) ) // TMM2764AD-20, red sticker
	ROM_LOAD("7014_blue",  0x4000, 0x2000, CRC(341d9ca6) SHA1(370876573bb9408e75f4fc797304b6c64af0590a) ) // TMM2764AD-20, blue sticker

	ROM_REGION( 0x0400, "mcu", 0 )
	ROM_LOAD("100-1009", 0x0000, 0x0400, CRC(60eb343f) SHA1(8a63e95ebd62e123bdecc330c0484a47c354bd1a) ) // NEC P07021-027 || D8041C 563 100-1009

	ROM_REGION( 0x1000, "speech", 0 )
	ROM_LOAD("101-32118", 0x0000, 0x1000, CRC(a0b8bb8f) SHA1(f56852108928d5c6caccfc8166fa347d6760a740) ) // ea 101-32118 || (C) 1980 || EA 8332A247-4 || 8034
ROM_END


ROM_START( damesc ) // model DSC, PCB label 510-1030A01
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "101-1027a01", 0x0000, 0x2000, CRC(d86c985c) SHA1(20f923a24420050fd16e1172f5e889f144d17ac9) ) // MOS 2364
ROM_END



/******************************************************************************
    Drivers
******************************************************************************/

//    YEAR  NAME      PARENT  CMP MACHINE INPUT  CLASS           INIT        COMPANY                 FULLNAME, FLAGS
CONS( 1978, cc10,     0,       0, ccx,    ccx,   ccx_state, empty_init, "Fidelity Electronics", "Chess Challenger 10 (model CCX, rev. B)", MACHINE_SUPPORTS_SAVE | MACHINE_CLICKABLE_ARTWORK )
CONS( 1979, cc7,      0,       0, bcc,    bcc,   bcc_state, empty_init, "Fidelity Electronics", "Chess Challenger 7 (model BCC, rev. B)", MACHINE_SUPPORTS_SAVE | MACHINE_CLICKABLE_ARTWORK )
CONS( 1979, backgamc, 0,       0, bkc,    bkc,   bcc_state, empty_init, "Fidelity Electronics", "Backgammon Challenger", MACHINE_SUPPORTS_SAVE | MACHINE_CLICKABLE_ARTWORK | MACHINE_NO_SOUND_HW )

CONS( 1980, fscc8,    0,       0, scc,    scc,   scc_state, empty_init, "Fidelity Electronics", "Sensory Chess Challenger 8", MACHINE_SUPPORTS_SAVE | MACHINE_CLICKABLE_ARTWORK | MACHINE_IMPERFECT_CONTROLS )

CONS( 1979, vcc,      0,       0, vcc,    vcc,   vcc_state, init_language<0>, "Fidelity Electronics", "Voice Chess Challenger (English)", MACHINE_SUPPORTS_SAVE | MACHINE_CLICKABLE_ARTWORK )
CONS( 1979, vccg,     vcc,     0, vcc,    vcc,   vcc_state, init_language<1>, "Fidelity Electronics", "Voice Chess Challenger (German)", MACHINE_SUPPORTS_SAVE | MACHINE_CLICKABLE_ARTWORK )
CONS( 1979, vccsp,    vcc,     0, vcc,    vcc,   vcc_state, init_language<2>, "Fidelity Electronics", "Voice Chess Challenger (Spanish)", MACHINE_SUPPORTS_SAVE | MACHINE_CLICKABLE_ARTWORK )
CONS( 1979, vccfr,    vcc,     0, vcc,    vcc,   vcc_state, init_language<4>, "Fidelity Electronics", "Voice Chess Challenger (French)", MACHINE_SUPPORTS_SAVE | MACHINE_CLICKABLE_ARTWORK )

CONS( 1980, uvc,      vcc,     0, vcc,    vcc,   vcc_state, init_language<0>, "Fidelity Electronics", "Advanced Voice Chess Challenger (English)", MACHINE_SUPPORTS_SAVE | MACHINE_CLICKABLE_ARTWORK )
CONS( 1980, uvcg,     vcc,     0, vcc,    vcc,   vcc_state, init_language<1>, "Fidelity Electronics", "Advanced Voice Chess Challenger (German)", MACHINE_SUPPORTS_SAVE | MACHINE_CLICKABLE_ARTWORK )
CONS( 1980, uvcsp,    vcc,     0, vcc,    vcc,   vcc_state, init_language<2>, "Fidelity Electronics", "Advanced Voice Chess Challenger (Spanish)", MACHINE_SUPPORTS_SAVE | MACHINE_CLICKABLE_ARTWORK )
CONS( 1980, uvcfr,    vcc,     0, vcc,    vcc,   vcc_state, init_language<4>, "Fidelity Electronics", "Advanced Voice Chess Challenger (French)", MACHINE_SUPPORTS_SAVE | MACHINE_CLICKABLE_ARTWORK )

CONS( 1980, vsc,      0,       0, vsc,    vsc, vsc_state, init_language<0>, "Fidelity Electronics", "Voice Sensory Chess Challenger (English)", MACHINE_SUPPORTS_SAVE | MACHINE_CLICKABLE_ARTWORK | MACHINE_IMPERFECT_CONTROLS )
CONS( 1980, vscg,     vsc,     0, vsc,    vsc, vsc_state, init_language<1>, "Fidelity Electronics", "Voice Sensory Chess Challenger (German)", MACHINE_SUPPORTS_SAVE | MACHINE_CLICKABLE_ARTWORK | MACHINE_IMPERFECT_CONTROLS )
CONS( 1980, vscsp,    vsc,     0, vsc,    vsc, vsc_state, init_language<2>, "Fidelity Electronics", "Voice Sensory Chess Challenger (Spanish)", MACHINE_SUPPORTS_SAVE | MACHINE_CLICKABLE_ARTWORK | MACHINE_IMPERFECT_CONTROLS )
CONS( 1980, vscfr,    vsc,     0, vsc,    vsc, vsc_state, init_language<3>, "Fidelity Electronics", "Voice Sensory Chess Challenger (French)", MACHINE_SUPPORTS_SAVE | MACHINE_CLICKABLE_ARTWORK | MACHINE_IMPERFECT_CONTROLS )

CONS( 1980, vbrc,     0,       0, vbrc,   vbrc,  card_state, empty_init, "Fidelity Electronics", "Voice Bridge Challenger", MACHINE_SUPPORTS_SAVE | MACHINE_CLICKABLE_ARTWORK | MACHINE_IMPERFECT_CONTROLS | MACHINE_NOT_WORKING )
CONS( 1980, bridgeca, vbrc,    0, ubc,    vbrc,  card_state, empty_init, "Fidelity Electronics", "Advanced Bridge Challenger", MACHINE_SUPPORTS_SAVE | MACHINE_CLICKABLE_ARTWORK | MACHINE_IMPERFECT_CONTROLS | MACHINE_NOT_WORKING )
CONS( 1982, bridgec3, 0,       0, bv3,    bv3,   card_state, empty_init, "Fidelity Electronics", "Bridge Challenger III", MACHINE_SUPPORTS_SAVE | MACHINE_CLICKABLE_ARTWORK | MACHINE_IMPERFECT_CONTROLS | MACHINE_NOT_WORKING )

CONS( 1981, damesc,   0,       0, dsc,    dsc,   dsc_state, empty_init, "Fidelity Electronics", "Dame Sensory Challenger", MACHINE_SUPPORTS_SAVE | MACHINE_CLICKABLE_ARTWORK | MACHINE_IMPERFECT_CONTROLS )
/* -*- c++ -*- */

/*
    Please _check and contribute_ : http://code.google.com/p/libadsb
*/

%include "gnuradio.i"			// the common stuff

%{
#include "adsb_modulate_modes.h"
#include <gr_msg_queue.h>
%}

// ----------------------------------------------------------------

/*
 * First arg is the package prefix.
 * Second arg is the name of the class minus the prefix.
 *
 * This does some behind-the-scenes magic so we can
 * access adsb_modulate_modes from python as adsb.modulate_modes
 */
GR_SWIG_BLOCK_MAGIC(adsb, modulate_modes);

adsb_modulate_modes_sptr adsb_make_modulate_modes (int channel_rate, gr_msg_queue_sptr queue, float threshold_db);

class adsb_modulate_modes : public gr_block
{
private:
	adsb_modulate_modes(int channel_rate, gr_msg_queue_sptr queue, float threshold_db);
};

// ----------------------------------------------------------------


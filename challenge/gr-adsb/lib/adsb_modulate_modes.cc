/*
    Please _check and contribute_ : http://code.google.com/p/libadsb
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <adsb_modulate_modes.h>
#include <gr_io_signature.h>
#include <string.h>
#include <iostream>
#include <gr_tags.h>

// Internal function
static double modulate_preamble()
{
}

adsb_modulate_modes_sptr adsb_make_modulate_modes(int channel_rate, gr_msg_queue_sptr queue, float threshold_db)
{
	return adsb_modulate_modes_sptr (new adsb_modulate_modes(channel_rate, queue, threshold_db));
}

adsb_modulate_modes::adsb_modulate_modes(int channel_rate, gr_msg_queue_sptr queue, float threshold_db) :
    gr_block (  "adsb_modulate_modes",
                gr_make_io_signature (1, 1, sizeof(float))) //the input _encoded_ packet/frame
                gr_make_io_signature (1, 1, sizeof(float))) //the output _modulated_ bits ready for RF transmission
{
    d_chip_rate = 2000000; //2Mchips per second
	d_samples_per_chip = channel_rate / d_chip_rate; //must be integer number of samples per chip to work
	d_samples_per_symbol = d_samples_per_chip * 2;
	d_check_width = 120 * d_samples_per_symbol; //how far you will have to look ahead
	d_queue = queue;
	
	d_threshold_db = threshold_db;
	d_threshold = powf(10., threshold_db/20.); //the level that the sample must be above the moving average in order to qualify as a pulse
	d_secs_per_sample = 1.0 / channel_rate;
}

int adsb_modulate_modes::general_work(
        int noutput_items,
        gr_vector_int &ninput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
{
    for(int i=0; i < ninputs; i++)
    {
        // CHALLENGE: The modulation simplistic logic goes here
    }
}

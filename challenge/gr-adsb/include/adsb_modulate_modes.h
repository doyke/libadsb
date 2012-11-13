/*
    Please _check and contribute_ : http://code.google.com/p/libadsb
*/

#ifndef INCLUDED_ADSB_MODULATE_MODES_H
#define INCLUDED_ADSB_MODULATE_MODES_H

#include <gr_block.h>

// Here begins some general static things
#define PREAMBLE_PULSE_1    0
#define PREAMBLE_PULSE_1    2
#define PREAMBLE_PULSE_1    7
#define PREAMBLE_PULSE_1    9

static const bool ADSB_MODES_PREAMBLE_BITS[10] = {0};
// Make it look like this bit-wise
//
// Indx:  0  1  2  3  4  5  6  7  8  9
//
// Bits: {1, 0, 1, 0, 0, 0, 0, 1, 0, 1};
static
{
    ADSB_MODES_PREAMBLE_BITS[PREAMBLE_PULSE_1] = 1;
    ADSB_MODES_PREAMBLE_BITS[PREAMBLE_PULSE_2] = 1;
    ADSB_MODES_PREAMBLE_BITS[PREAMBLE_PULSE_3] = 1;
    ADSB_MODES_PREAMBLE_BITS[PREAMBLE_PULSE_4] = 1;
};

// Internal function
static double modulate_preamble();

// Here begins the modulator class definition
class adsb_modulate_modes;
typedef boost::shared_ptr<adsb_modulate_modes> adsb_modulate_modes_sptr;

adsb_modulate_modes_sptr adsb_modulate_modes(int channel_rate, gr_msg_queue_sptr queue, float threshold_db);

/*!
 * \brief ADSB 1090ES ModeS - Modulation block
 * \ingroup block
 */
// We need gr_block and not gr_sync_block, since we will be prepending preamble 
// so we need both noutput_items and ninput_items
class adsb_modulate_modes : public gr_block
{
private:
    friend adsb_modulate_modes_sptr adsb_make_modulate_modes(int channel_rate, gr_msg_queue_sptr queue, float threshold_db);
    adsb_modulate_modes(int channel_rate, gr_msg_queue_sptr queue, float threshold_db);

    int d_check_width;
    int d_chip_rate;
    int d_samples_per_chip;
    int d_samples_per_symbol;
    float d_threshold;
    gr_msg_queue_sptr d_queue;
    std::ostringstream d_payload;
    
    float d_threshold_db;
	float d_threshold;
	double d_secs_per_sample;

public:
    // Since we will be prepending preamble, noutput_items will be > ninput_items
    int general_work (
            int noutput_items,
            gr_vector_int &ninput_items,
            gr_vector_const_void_star &input_items,
            gr_vector_void_star &output_items
    );
};

#endif /* INCLUDED_ADSB_MODULATE_MODES_H */

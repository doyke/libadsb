"""
    Please _check and contribute_ : http://code.google.com/p/libadsb
"""

// Other imports ommitted for brevity

// This is the GNURadio ADSB _modulators_ interfaced to python via SWIG in adsb.i
import adsb
// This it the ASN.1 based _encoder_/_decoder_ library interfaced to python via SWIG
import pylibadsb

class top_block_runner(_threading.Thread):
    def __init__(self, tb):
        _threading.Thread.__init__(self)
        self.setDaemon(1)
        self.tb = tb
        self.done = False
        self.start()

    def run(self):
        self.tb.run()
        self.done = True

class adsb_rx_block (gr.top_block):
    def __init__(self, options, args, queue):
        gr.top_block.__init__(self)
        
        self.options = options
        self.args = args
        rate = int(options.rate)

        self.adsb_tx_path = adsb.adsb_modes_tx_path(rate, options.threshold, queue)
        
        // We want a sink where the modulated data will be transmitted to
		self.uhd_usrp_sink_0 = uhd.single_usrp_sink(
			device_addr="",
			// This is important, since we will use gr.float_to_complex()
			io_type=uhd.io_type_t.COMPLEX_FLOAT32,
			num_channels=1,
		)
		self.uhd_usrp_sink_0.set_samp_rate(rate)
		self.uhd_usrp_sink_0.set_center_freq(options.freq, 0)
		self.uhd_usrp_sink_0.set_gain(10, 0)
		self.uhd_usrp_sink_0.set_antenna("TX/RX", 0)
		time_spec = uhd.time_spec(0.0)
        self.uhd_usrp_sink_0.set_time_now(time_spec)
		
		// And now we connect all the blocks together
		self.connect(self.adsb_tx_path, self.uhd_usrp_sink_0)

if __name__ == '__main__':
    // Usage message & option parsing
    (options, args) = parser.parse_args()
    
    // This creates the GnuRadio graph based on a block (inside the block is all the glue going on)
    gr_graph = adsb_rx_block(options, args, queue)
    
    // This creates the thread which keeps the block running in the graph
    runner = top_block_runner(gr_graph)

    while 1:
        try:
            // CHALLENGE: Use libadsb to encode some payload
            // Example: let's use DF17 for simplicity
            me_payload = [0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41]
            // Here we will have a byte array of encoded data for DF17 with parity information (PI) computed
            adsb_modes_enc_msg = libadsb.create_by_type(type = libadsb.DF_types.DF17, ca = 0x6, aa = 0xdeadbf, me = me_payload).encode()
            
            // Inject the libadsb encoded messages into the message queue for adsb_modes_tx_path to consume
        except KeyboardInterrupt:
            gr_graph.stop()
            runner = None
            break

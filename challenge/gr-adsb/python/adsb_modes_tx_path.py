"""
    Please _check and contribute_ : http://code.google.com/p/libadsb
"""

from gnuradio import gr
import adsb

class adsb_modes_tx_path(gr.hier_block2):
    def __init__(self, rate, threshold, queue):
        gr.hier_block2.__init__(self, "adsb_modes_tx_path",
                                gr.io_signature(0,0,0),
                                gr.io_signature(1, 1, gr.sizeof_gr_complex)
                                )

        self._rate = int(rate)
        self._threshold = threshold
        self._queue = queue

        # Convert outgoing amplitudes to I/Q baseband (raw modulation)
        self._mod_raw = gr.float_to_complex()

        # Modulate ADSB ModeS packet bits and send down the flowgraphs
        self._mod_adsb = adsb.modulate_adsb(self._rate, self._queue, self._threshold)

        # Wire up the flowgraph
        self.connect(self, self._mod_adsb)
        self.connect(self._mod_adsb, self._mod_raw)

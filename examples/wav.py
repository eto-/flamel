import wave as wav
import numpy as np
from collections import namedtuple

class WAV:
    def __init__(self, conf):
        try:
          self.file_name = conf['file']
        except KeyError:
          self.file_name = 'none'
        self.dummy = self.file_name == 'none'
        if self.dummy: return

        self.writing = conf.get('write', 'false').lower() in [ 'true', '1', 'yes' ]
        if self.writing:
            self.format = float(conf.get('wav_format', '2.0'))
            self.sampling = float(conf.get('sampling', 1e9))
            self.rate = float(conf.get('rate', '100'))
            self.file = None
        else:
            self.HEADER = namedtuple('HEADER', 'marker header_length counter time_tag n_samples cpu_time_ms n_channels version')
            self.DATA = namedtuple('DATA', 'id wav')
            self.file = wav.open(self.file_name, 'rb')
            assert self.file.getnchannels() == 1, "format error (number of channels = %d)" % self.file.getnchannels() 
            assert self.file.getsampwidth() == 2, "format error (sample width = %d)" % self.file.getsampwidth()
            self.sampling = self.file.getframerate() * 1000000

    def __del__(self):
        if not self.dummy and self.file != None:
            self.file.close()

    def write(self, e, ws):
        if self.dummy: return
        if not self.writing: raise AttributeError('Trying to write to a read-only file')
        if self.file == None:
            self.file = wav.open(self.file_name, 'wb')
            self.file.setnchannels(1)
            self.file.setsampwidth(2)
            self.file.setframerate(int(self.sampling)/1e6)
            self.file.setcomptype('NONE', 'not compressed')
            self.counter = 0
            self.time = 0

        
        if e is not None and set(['counter', 'time_tag', 'cpu_time_ms', 'version']).issubset(e._fields):
            if self.counter == 0:
                self.format = float(e.version)
            time_tag = e.time_tag
            cpu_time_ms = e.cpu_time_ms
            counter = e.counter
        else:
            if e is None:
                self.time += np.random.exponential(1 / self.rate)
                self.counter += 1
            else:
                self.time = e.time
                self.counter = e.id
            time_tag = int(self.time / 8e-9) % 0xFFFFFFFF
            cpu_time_ms = int(round(self.time * 1e3))
            counter = self.counter


        if self.format >= 2:
            #uint16_t marker, header_length;
            #uint32_t counter, time_tag, n_samples, cpu_time_ms;
            #uint16_t n_channels, version;
            #uint32_t data_length, unused[3]
            data_len = sum(map(lambda w: 4 + len(w.wav), ws))
            f = wav.struct.pack('=2H4I2H4I', *(0xffff, 20, counter, time_tag, 0, cpu_time_ms, len(ws), 200, data_len, 0, 0, 0))
            for w in ws:
                #uint16_t channel, n_samples, unused[2];
                f = f + wav.struct.pack('=4H', *(w.id, len(w.wav), 0, 0))
                f = f + wav.struct.pack("=%dh" % len(w.wav), *w.wav)
        else:
            # uint16_t marker, header_length;
            # uint32_t counter, time_tag, n_samples, cpu_time_ms, n_channels, unused[4];
            # uint16_t samples[0];
            f = wav.struct.pack('=2H9I', *(0xffff, 20, counter, time_tag, len(ws[0].wav), cpu_time_ms, len(ws), 0, 0, 0, 0)) 
            for w in ws:
                if len(w.wav) != len(ws[0].wav):
                    raise ValueError('wav formats earlier than 2.0 require constant waveforms for the same event')
                f = f + wav.struct.pack("=%dh" % len(w.wav), *w.wav)

        self.file.writeframes(f)


    def read(self):
        if self.dummy: return
        if self.writing: raise AttributeError('Trying to read from a write-only file')

        h = self.HEADER(*self.__read('=2H4I2H4I')[0:8])
        assert h.marker == 0xffff, 'marker not found %d' % h.marker
        assert h.header_length == 20, 'bogus header length %d' % h.header_length

        w = []

        if h.n_samples == 0:
            for i in range(h.n_channels):
                c_h = self.__read('=4H')
                c_w = self.__read('=%dH' % c_h[1])
                w.append(self.DATA(c_h[0], c_w))
        else:
            for i in range(h.n_channels):
                c_w = self.__read('=%dH' % h.n_samples)
                w.append(self.DATA(i, c_w))

        return (h, w)
    
    def __read(self, fmt):
        l = wav.struct.calcsize(fmt)
        b = self.file.readframes(int(l / 2))
        if len(b) != l:
            raise EOFError
        return wav.struct.unpack(fmt, b)


    def __next__(self):
        if self.dummy: return self.read()
        try:
            return self.read()
        except EOFError:
            raise StopIteration

    def __iter__(self):
        return self


if __name__ == "__main__":
    import sys
    import numpy as np
    from statistics import mean
    import matplotlib.pyplot as plt

    f = WAV({'file':sys.argv[1]})
    v_min = []
    for i in f:
        #print(i[0])
        w = i[1][0].wav
        b = mean(w[2800:2900])
        v_min.append(b - min(w[2950:3050]))

    h_min = plt.hist(v_min, list(range(0,1024)))
    plt.show()



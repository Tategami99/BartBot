import wave
import struct
import os
import sys

def wav_to_header(wav_filename, header_filename, array_name):
    #Open wav file
    with wave.open(wav_filename, 'rb') as wav_file:
        n_channels, sampwidth, framerate, n_frames, _, _ = wav_file.getparams()
        frames = wav_file.readframes(n_frames * n_channels)
        samples = struct.unpack_from("%dh" % n_frames * n_channels, frames)

    #Write to header file
    with open(header_filename, 'w') as header_file:
        #Begin header guard
        header_file.write("#ifndef {}_H\n".format(array_name.upper()))
        header_file.write("#define {}_H\n\n".format(array_name.upper()))

        #Include libraries
        header_file.write("#include <Arduino.h>\n\n")

        #Write array definition
        header_file.write("const int16_t {}[] PROGMEM = {{\n".format(array_name))
        #Write samples to array
        for i, sample in enumerate(samples):
            if i % 12 == 0:
                header_file.write("\n    ")  #Newline every 12 samples to look nicer
            header_file.write("{}, ".format(sample))
        header_file.write("\n};\n")
        #Write size of array
        header_file.write("#define {}_SIZE {}\n".format(array_name.upper(), len(samples)))
        
        #End header guard
        header_file.write("\n#endif // {}_H\n".format(array_name.upper()))

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Do: python wav_to_header.py <path_to_wav_file>")
        sys.exit(1)

    wav_filename = sys.argv[1]
    header_filename = os.path.splitext(wav_filename)[0] + ".h"
    array_name = os.path.splitext(os.path.basename(wav_filename))[0].replace(" ", "_").replace("-", "_")

    #Convert wav file to header file
    wav_to_header(wav_filename, header_filename, array_name)

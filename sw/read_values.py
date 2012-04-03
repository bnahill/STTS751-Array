import struct
from math import isnan
import sys
def read_data(fname):
	with open(fname) as f:
		a = f.read()

	num_sensors = struct.unpack_from('i',a)[0]
	a = a[4:]
	out = list()
	for i in range(num_sensors):
		out.append(list())
	exit=False
	while(len(a) >= 32):
		for (f,arr) in zip(struct.unpack_from('f'*num_sensors, a), out):
			if isnan(f):
				exit = True
				break
			arr.append(f)
		a = a[num_sensors:]
		if exit:
			break
	return out

data=list()

if __name__ == '__main__':
	try:
		data = read_data(sys.argv[1])
	except IndexError:
		sys.exit("Please provide a filename to read")

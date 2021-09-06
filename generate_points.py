import random
import argparse
import os

ROOT_DIR = os.path.dirname(os.path.abspath(__file__))
INPUT_DIR = os.path.join(ROOT_DIR, "data")


def generate_points(length):
	random.seed(42)
	filename = "input_{}.csv".format(length)
	filepath = os.path.join(INPUT_DIR, filename)
	with open(filepath, "w") as f:
		for _ in range(length):
			x = round(random.uniform(0, 100), 2)
			y = round(random.uniform(0, 100), 2)
			f.write('{} {}\n'.format(x, y))


def check_positive(value):
	i_value = int(value)
	if i_value <= 0:
		raise argparse.ArgumentTypeError("%s is an invalid positive int value" % value)
	return i_value


if __name__ == '__main__':

	if not os.path.exists(INPUT_DIR):
		os.mkdir(INPUT_DIR)

	parser = argparse.ArgumentParser(description='Generate random 2D-points coordinates')
	parser.add_argument('len', help='number of points to generate', type=check_positive)
	args = parser.parse_args()

	generate_points(length=args.len)

	print("%d points generated." % args.len)

import subprocess
from subprocess import Popen, PIPE

print("running")
arguments = ["train", "test"]
for arg in arguments:
	p = subprocess.Popen(["./comp_label", arg], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
	p.communicate()

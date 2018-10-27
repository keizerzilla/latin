"""
feats.py
Artur Rodrigues Rocha Neto
2018

Funções para extração de momentos. Até o momento, o padrão de arquivo Bosphorus
é um único suportado. Mudanças futuras: FRGC e EUROCOM
"""

import os
import sys
import time
import parse
import subprocess

"""
Executa extração de momentos em uma nuvem e parsing das informações de amostra e 
classe (padrão da base Bosphorus). Por enquanto apenas nuvem inteira, depois
mudar para receber tipo de corte como argumento.

moment -- O momento a ser usado
cloud -- A nuvem alva

return -- um string com os momentos seguidos de amostra e classe (CSV)
"""
def moment_extraction_cloud(moment, cloud):
	mcalc_exec = "../bin/mcalc"
	parse_fmt = "bs{:d}_{}_{}_{:w}.xyz"
	path, filename = os.path.split(cloud)
	
	cl = str(parse.parse(parse_fmt, filename)[0])
	ix = str(parse.parse(parse_fmt, filename)[3])
	
	cmd = [mcalc_exec, "-m", moment, "-i", cloud, "-o", "stdout", "-c", "w"]
	ans = subprocess.run(cmd, check=True, stdout=subprocess.PIPE).stdout
	ans = ans.decode("utf-8")[:-2].replace(" ", ",") + ",{},{}\n".format(ix, cl)
	
	return ans

"""
Executa extração de momentos em um conjunto de nuvens, além do parsingo de 
informações de amostra e classe (padrão Bosphorus).

moment -- O momento a ser usado
dataset -- A pasta com as nuvens
output -- O arquivo de saída aonde os momentos serão salvos
"""
def moment_extraction_batch(moment, dataset, output):
	with open(output, "w") as dump:
		for cloud in os.listdir(dataset):
			if cloud.endswith(".xyz"):
				full_path = dataset + "/" + cloud
				ans = moment_extraction_cloud(moment, full_path)
				dump.write(ans)
				print("[{}] >> {}: OK".format(cloud, moment))

if __name__ == "__main__":
	scenarios = ["bosphorus-c40",
	             "bosphorus-c45",
	             "bosphorus-c50",
	             "bosphorus-c55",
	             "bosphorus-c60",
	             "bosphorus-c65",
	             "bosphorus-c70",
	             "bosphorus-c75",
	             "bosphorus-c80",
	             "bosphorus-c85",
	             "bosphorus-c90"]
	moments = ["zernike", "legendre", "chebyshev", "hututu", "hu1980"]
	datasets = ["../datasets/" + x + "/neutral" for x in scenarios]
	
	for data in datasets:
		scenario = data.split("/")[2]
		folder = "../results/{}".format(scenario)
		
		try:
			os.mkdir(folder)
		except:
			print("OPS! Diretorio jah existe!")
		
		for moment in moments:
			out = folder + "/{}-{}.dat".format(os.path.split(data)[1], moment)
			moment_extraction_batch(moment, data, out)
	

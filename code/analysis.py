import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

data=pd.read_csv("overallinfo.csv")
data.fillna(0)
column_names=["time","cpu_utilization","processor_time","user_time","nice_time","idle_time","memory_utilization","used_mem","total_mem","free_mem","available_mem","pagetable","vmused","num_pages","pf_before","pf_persec","ps_in_persec","ps_out_psec","pread_persec","pwrite_persec","p_persec"]
data=pd.read_csv("overallinfo.csv",names=column_names)
del data["p_persec"]
del data["ps_in_persec"]
del data["ps_out_psec"]
data['time']=pd.to_datetime(data['time'])
#print(data)

fig,ax=plt.subplots(2,3)
c=[data['processor_time'].mean(),data['user_time'].mean(),data['idle_time'].mean()]
my_labels="processor time","user time","idle time"
ax[0,0].pie(c,labels=my_labels,autopct='%1.1f%%')
ax[0,0].set_title("CPU UTILIZATION")

c1=[data['used_mem'].mean(),data['available_mem'].mean()]
m_labels="used memory","available memory"
ax[0,1].pie(c1,labels=m_labels,autopct='%1.1f%%')
ax[0,1].set_title("MEMORY AVAILABLE")

data.plot.scatter(x='time',y='processor_time',ax=ax[0,2])
ax[0,2].set_title("PROCESSOR TIME")
data.plot.scatter(x='time',y='cpu_utilization',ax=ax[1,0])
ax[1,0].set_title("CPU UTILIZATION")

data.plot.line(x='time',y='memory_utilization',ax=ax[1,1])
ax[1,1].set_title("MEMORY UTILIZATION")
data.plot.area(alpha=0.5,y=['idle_time','processor_time','user_time','nice_time'],x='time',ax=ax[1,2])
ax[1,2].set_title("OVERALL CPU USAGE")

#input("Press any key to continue")

plt.show()

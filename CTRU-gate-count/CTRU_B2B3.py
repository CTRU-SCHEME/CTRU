from cost import *


# Result of the DDGR20 BKZ prediction
#
# ======= CTRU-512-B2
# GSA Intersect:               dim=1007   δ=1.004132      β=379.31 
# Probabilistic simulation:    dim=1007                   β=386.70
# ======= CTRU-768-B2
# GSA Intersect:               dim=1467   δ=1.002915      β=620.30
# Probabilistic simulation:    dim=1467                   β=634.25
# ======= CTRU-1024-B2
# GSA Intersect:               dim=1919   δ=1.002271      β=869.14  
# Probabilistic simulation:    dim=1919                   β=890.12 

# ======= CTRU-512-B3
# GSA Intersect:               dim=1025   δ=1.003961      β=403.21 
# Probabilistic simulation:    dim=1025                   β=411.10
# ======= CTRU-768-B3
# GSA Intersect:               dim=1498   δ=1.002796      β=656.52
# Probabilistic simulation:    dim=1498                   β=671.31
# ======= CTRU-1024-B3
# GSA Intersect:               dim=1958   δ=1.002180      β=917.72  
# Probabilistic simulation:    dim=1958                   β=939.78 

print("              \t& n   \t&  β \t& β' \t& gates \t& memory ")
print("CTRU-512-B2 \t& %d\t& %d\t& %d\t& %.1f \t& %.1f "%summary(1007, 386))
print("CTRU-768-B2 \t& %d\t& %d\t& %d\t& %.1f \t& %.1f "%summary(1467, 634))
print("CTRU-1024-B2 \t& %d\t& %d\t& %d\t& %.1f \t& %.1f "%summary(1919, 890))

print("CTRU-512-B3 \t& %d\t& %d\t& %d\t& %.1f \t& %.1f "%summary(1007, 411))
print("CTRU-768-B3 \t& %d\t& %d\t& %d\t& %.1f \t& %.1f "%summary(1467, 671))
print("CTRU-1024-B3 \t& %d\t& %d\t& %d\t& %.1f \t& %.1f "%summary(1919, 939))

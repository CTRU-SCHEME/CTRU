from scipy.stats import chi2
from math import sqrt, log, ceil, erf
from math import factorial as fac
from proba_util import *


''' Calculate qs/2 in v and the error after decompression'''
def build_rounding_law_message(ps):
    D = {}
    for k1 in range(0, ps.p):    
        temp = ps.q1/ps.p*k1
        epsilon = round( temp ) - temp
        D[epsilon] = D.get(epsilon,0)+1./ps.p    
    return D   
        

''' Calculate v and the error after decompression  '''
def build_rounding_law_ciphertext(ps):
    D = {}
    for sigma1 in range(0, ps.q1):
        for k1 in range(0, ps.p):    
            temp = 1.*ps.q2/ps.q1*( sigma1 + round(ps.q1/ps.p*k1))
            if temp >= ps.q2:
                temp -= ps.q2
            if temp <= -ps.q2:
                temp += ps.q2                
            epsilon = round( temp ) - temp
            if epsilon >= ps.q2/2:
                epsilon -= ps.q2
            if epsilon <= -ps.q2/2:
                epsilon += ps.q2              
            D[epsilon] = D.get(epsilon,0)+1./ps.q1 * 1./ps.p    
    return D  


def ErrorRate(ps):
    sigma_epsilon1 = sqrt( var_of_law( build_rounding_law_message(ps) ) )    
    sigma_epsilon2 = sqrt( var_of_law( build_rounding_law_ciphertext(ps) ) )
    s1 = ps.n/2*(ps.sigma1**2 * ps.sigma2**2 + ps.sigma2**2 * (ps.sigma1**2+ps.sigma1**2) ) 
    temp = ps.sigma2**2+sigma_epsilon1**2+(ps.q1/ps.q2*sigma_epsilon2)**2
    s2 = ps.n/2*(ps.p**2)*(temp * ps.sigma1**2 + temp * (ps.sigma1**2+ps.sigma1**2) ) 
    s3 = ps.sigma2**2+sigma_epsilon1**2+(ps.q1/ps.q2*sigma_epsilon2)**2  
    s = sqrt(s1 + s2 + s3)
    pr = chi2.logsf( (ps.threshold/s)**2, 8 ) / log(2) 
    print("err:")
    print("    = 2^%.2f"% pr)


def Bandwidth(ps):
    pk = int(ps.n*ceil(log(ps.q1,2))/8)
    ct = int(ps.n*ceil(log(ps.q2,2))/8)
    print('|pk| = %d, |ct| = %d, bandwidth = %d\n'%(pk, ct, (pk+ct)))
    
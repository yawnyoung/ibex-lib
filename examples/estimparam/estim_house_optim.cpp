#include "ibex.h"

#include <iostream>
#include <stdlib.h>
#include <ctime>
#include <fstream>
#include <set>
using namespace std;
using namespace ibex;

 

/*
 * 
 * Detects plan in real 3d point clouds using q-intersection. 
 * 
 * 
 * 
 */


	 
int main(int argc, char** argv) {
	
	vector<double> *x = new vector<double>;
	vector<double> *y = new vector<double>;
	vector<double> *z = new vector<double>;


	double cputime =0;
	double totaltime=0;
	string input_file_name=argv[1];
        double epseq=atof(argv[2]);
	int Q= atoi(argv[3]);
	double precbc= atof(argv[4]);
	double precd=atof(argv[5]);
	int gap= atoi(argv[6]);
	int nbrand = atoi(argv[7]);

	cout << input_file_name << endl;
	ifstream input(input_file_name.c_str());
	while (!input.eof())
	  {double in;
	    input >> in;
	    x->push_back(in);
	    //	    cout << in << " " ;
	    input >> in;
	    y->push_back(in);
	    //	    cout << in << " " ;
	    input >> in;
            z->push_back(in);
	    //	    cout << in << " " << endl;
            
	  }
	x->pop_back();y->pop_back();z->pop_back();
	srand(atoi(argv[8]));

	cout << " nb points " << x->size() << endl;
	//	for (int i=0; i< x->size(); i++) cout << (*x)[i] <<  " " << (*y)[i] << " " << (*z)[i] <<endl;
	
	

	int n=3;
	
	Variable u(3);
	Variable w(3);
	
	int p = x->size();
	int nz=0;
	for (int i=0;i<p;i++) {if ((*z)[i] >=2.79 && (*z)[i] <3) nz++;      }
	cout << "nz " << nz << endl;
	  

	//	int Q2= Q*Qprop;
	int Q1=27;
	//	int Q1=9;
	//	int Q2= 4;
	int Q2= 3;
	//	int NT2= 5.01 / (p * (pow (Qprop,3)));
	//	int NT2= 4.5 / (p * (pow (Qprop,3))) + 1;
	int NT1=3;
	int NT2=5;
	//	int NT1= 12.01 / (p * (pow (Qprop,2)));
	//int NT1= 9.01 / (p * (pow (Qprop,2)));
	int K=1;
	int np;

	if (K==1)np=p;
	else if (K==2) {np=NT1*p;Q=Q1;}
	else {np=NT2*p;Q=Q2;}



	Function ***m_fun;

	double ** linfun;

	linfun = new double*[p];
	for (int i=0; i<p; i++)
	  {  linfun[i] = new double[n];
	  }

	

          



	Array<Ctc> m_ctc(p);
	Array<Ctc> m_ctc1(np);

	Function* m_func[p] ;	
        Function* m_f0;
	Ctc* ctc0;
	Variable v(3);
	double R[p];
	double alpha[p];
	clock_t start, start0, end;
	int nb_cells=0;
	start = clock();
	start0=clock();
	int Q0=Q;

	int Qvalid=Q;
	cout << np << "  " << Q0 << " " << Qvalid << " " << epseq << endl;

	int Qoct=Q;
	Vector bestsol (3);
	for (int oct=0; oct <4; oct++)
	  { int diry= pow(-1,oct%2);
	    int dirz= pow(-1,oct/2);
	    m_f0= new Function(v, (diry*v[0]+dirz*v[1]-1));
            ctc0=new CtcFwdBwd(*m_f0,LEQ);
	    for (int i=0; i<p; i++) {
	  /*		
	  R[i] = sqrt( pow(x->at(i),2)+pow(y->at(i),2));
	  alpha[i] = atan(y->at(i)/x->at(i));
	  m_func[i] = new Function(v,sqrt(1-sqr(v[0]))*R[i]*cos(v[1]-alpha[i])+v[0]*z->at(i)-v[2]+Interval(-epseq,epseq));
	  */
	      
	      m_func[i] = new Function(v,(x->at(i) +v[0]*(y->at(i)- diry*x->at(i))+v[1]*(z->at(i)-dirz*x->at(i))-v[2]-Interval(-epseq,epseq)));
	      m_ctc.set_ref(i,(*new CtcFwdBwd(*m_func[i])));
	  
	  //	  m_ctc.set_ref(i,*new CtcCompo (*new CtcFwdBwd(*m_func[i]), * new CtcFwdBwd(*m_func[i], EQ, AFFINE2_MODE)));
	  //	  m_ctc.set_ref(i, (* new CtcFixPoint (* new CtcFwdBwd(*m_func[i]),0.2)));
	  //	  m_ctc.set_ref(i,*new Ctc3BCid(3,* new  CtcFwdBwd(*m_func[i]),5,1,3));  
	  //m_ctc.set_ref(i,*new CtcCompo (*new CtcFixPoint (* new CtcFwdBwd(*m_func[i]),0.2),  // cid seul ne marche pas
	  //					 *new Ctc3BCid(3,* new CtcFixPoint (* new CtcFwdBwd(*m_func[i]),0.2),5,1,3)));
	  //	  m_ctc.set_ref(i,*new CtcCompo (* new CtcFwdBwd(*m_func[i]),  // cid seul ne marche pas
	  //					 *new Ctc3BCid(3, * new CtcFwdBwd(*m_func[i]),5,1,3)));
	    }


	    if (K==1) {
	      for (int i=0; i<p; i++)  {
	    /* We must be on the plane defined by v */
		m_ctc1.set_ref(i,m_ctc[i]);
		//		m_fun[0][i]= new Function(v,(x->at(i) +v[0]*(y->at(i)- diry*x->at(i))+v[1]*(z->at(i)-dirz*x->at(i))-Interval(-epseq,epseq)));
		linfun[i][0]=x->at(i);
		linfun[i][1]=y->at(i)-diry*x->at(i);
		linfun[i][2]=z->at(i)-dirz*x->at(i);
	      }
	    }
	    /*
	    else if(K==2)
	      {
		for (int nt=0; nt<NT1;nt++)
		  for (int i=0; i<p; i++) {
		    int j = rand() % (p-1);
		    if (j>=i) j=j+1;
		    m_ctc1.set_ref(p*nt+i,*new CtcFixPoint (* new CtcCompo (m_ctc[i],m_ctc[j]),0.1));
		//		m_fun[0][p*nt+i]=new Function(v,(x->at(i) +diry*v[0]*(y->at(i)-x->at(i))+dirz*v[1]*(z->at(i)-x->at(i))-Interval(-epseq,epseq)));
		    
		    linfun[p*nt+i][0][0]=x->at(i);
		    linfun[p*nt+i][1][0]=y->at(i)-diry*x->at(i);
		    linfun[p*nt+i][2][0]=z->at(i)-dirz*x->at(i);

		//		m_fun[1][p*nt+i]=new Function(v,(x->at(j) +diry*v[0]*(y->at(j)-x->at(j))+dirz*v[1]*(z->at(j)-x->at(j))-Interval(-epseq,epseq)));	 
		    
		    linfun[p*nt+i][0][1]=x->at(j);
		    linfun[p*nt+i][1][1]=y->at(j)-diry*x->at(j);
		    linfun[p*nt+i][2][1]=z->at(j)-dirz*x->at(j);
		  }
	      }
	    else if(K==3)
	      {
		for (int nt=0; nt<NT2;nt++)
		  for (int i=0; i<p; i++) {
		    int j = rand() % (p-1);
		    int k = rand() % (p-2);
		    if (j>=i) j=j+1;
		    if (k >=j && k >= i) k=k+2;
		    else
		      {if (k >=j || k>=i ) k=k+1;
			if (k==i || k==j) k=k+1;
		      }
		    m_ctc1.set_ref(p*nt+i,*new CtcFixPoint (* new CtcCompo (m_ctc[i],m_ctc[j],m_ctc[k]),0.1));
		  //		  m_fun[0][p*nt+i]= new Function(v,(x->at(i) +diry*v[0]*(y->at(i)-x->at(i))+dirz*v[1]*(z->at(i)-x->at(i))-Interval(-epseq,epseq)));
		  //		  m_fun[1][p*nt+i]= new Function(v,(x->at(j) +diry*v[0]*(y->at(j)-x->at(j))+dirz*v[1]*(z->at(j)-x->at(j))-Interval(-epseq,epseq)));
		  //		  m_fun[2][p*nt+i]= new Function(v,(x->at(k) +diry*v[0]*(y->at(k)-x->at(k))+dirz*v[1]*(z->at(k)-x->at(k))-Interval(-epseq,epseq)));

		    
		    linfun[p*nt+i][0][0]=x->at(i);
		    linfun[p*nt+i][1][0]=y->at(i)-diry*x->at(i);
		    linfun[p*nt+i][2][0]=z->at(i)-dirz*x->at(i);

		    linfun[p*nt+i][0][1]=x->at(j);
		    linfun[p*nt+i][1][1]=y->at(j)-diry*x->at(j);
		    linfun[p*nt+i][2][1]=z->at(j)-dirz*x->at(j);

		    linfun[p*nt+i][0][2]=x->at(k);
		    linfun[p*nt+i][1][2]=y->at(k)-diry*x->at(k);
		    linfun[p*nt+i][2][2]=z->at(k)-dirz*x->at(k);
		  }

	      }
	    */

	    double _box[3][2];

	    if (diry==1) {_box[0][0] = 0;
	      _box[0][1] = 1;}
	    else {_box[0][0] = -1;_box[0][1] = 0;}
	    if (dirz==1)
	      {_box[1][0] = 0; _box[1][1] = 1;}
	    else
	      {_box[1][0] = -1; _box[1][1] = 0;}
	    _box[2][0]=-100;
	    _box[2][1]=100;
	
	    IntervalVector box(3,_box);

	    vector<IntervalVector> resgroup;


	    Vector prec(3);
	   
	    prec[0]=precbc;
	    prec[1]=precbc;
	    prec[2]=precd;
	    


	    cout << "precision bissection " <<  prec[0] << " " << prec[1] << " " << prec[2] << endl;
	    /*
	    prec[0]=0.001;
	    prec[1]=0.001;
	    prec[2]=0.001;
	    */
	    Vector proba(3);
	    proba[0]=0.33;
	    proba[1]=0.33;
	    proba[2]=0.34;
	    //	    CellStack buff;
	    // DepthFirstSearch str (buff);
	    CellHeapQInter buff;
	    BeamSearch str(buff);
	    //	    BestFirstSearch str(buff);
	    //RoundRobin bs (prec,0.5);
	    RoundRobinNvar bs (2,prec,0.5);
	    //  RoundRobinQInter bs (2,prec,0.5);
	    //	ProbaBisect bs (prec, proba, 0.45);
	    //LargestFirst bs (prec,0.45);
	    /* Main optimization loop */

	    //CtcQInterProjF ctcq(3,m_ctc1,Q);
	    //	CtcQInterProjF ctcq(3,m_ctc1,K,m_fun,Q);
	    
	    //	    CtcQInterAff ctcq(3,m_ctc1,Q,m_fun,QINTERPROJ,K);
	    CtcQInterAffPlane ctcq(n,p,m_ctc1,linfun,epseq,Qoct,QINTERPROJ,K);
	    	    
	    // CtcQInter2Plane ctcq(n,m_ctc1,linfun,epseq,Q);
	    Ctc3BCid cid(3,ctcq,5,1,1);
	    CtcCompo ctcid(ctcq ,cid);
            CtcCompo ctcqf0(*ctc0,ctcq);

	    CtcFixPoint ctcf(ctcqf0, 1);
	    //CtcFixPoint ctcf(ctcqf0, 0.1);
	    //CtcFixPoint ctcf(ctcid,0.1);


	    
	    SolverOptQInter s(ctcf,bs,str,ctcq,1);
	    //	    SolverOptQInter s(ctcf,bs,str,ctcq,2);

	    s.str.with_oracle=false;
	    s.str.with_storage=true;
	    s.time_limit = 3600;
	    s.trace=1;
	    s.gaplimit=gap;
	    s.nbr=nbrand;

	    s.bestsolpointnumber=Qoct;
	    s.bestsolpoint=bestsol;
	    cout << " avant resolution " << endl;

	   IntervalVector res=s.solve(box);

	cout << "Number of branches : " << s.nb_cells << endl;
	nb_cells +=s.nb_cells;
	cputime += s.time;
	Qoct=s.bestsolpointnumber;
	bestsol=s.bestsolpoint;
	s.report_possible_inliers();
	//	s.report_solution();


  


	

	end = clock();
	totaltime += ((double)(end)-(double)(start))/CLOCKS_PER_SEC;
	start= clock();

	  }

	
	for (int i=0; i<p; i++)
	  {delete m_func[i];
	    delete &m_ctc[i];
	  }

	  
	end=clock();
	cout << "Shape extraction : OK. Time : " << ((double)(end)-(double)(start0))/CLOCKS_PER_SEC << " seconds" << endl;
	cout << " total time " << totaltime << endl;
	cout << " cpu time " << cputime << endl;
	cout <<" total branch mumber " << nb_cells << endl;

	  				       
	for (int i=0; i<p; i++)
	      delete [] linfun[i];
	delete [] linfun;


	
}







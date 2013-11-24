/*********************************************************************************
*                                ɽè�ɿأ�Lynx��
*                             for LynxFly under GPLv2
*
* COPYRIGHT (C) 2012 - 2013, Lynx 84693469@qq.com
*
* Version   	: V1.0
* By        	: Lynx@sia 84693469@qq.com
*
* For       	: Stm32f405RGT6
* Mode      	: Thumb2
* Description   : �ջ�����������С���˱�ʶ��װ������ֱ�ӷ���У����������PID��������
*
*				
* Date          : 2013.XX.XX
*******************************************************************************/
#include "CFFRELS.h"
#include "..\Math\LibMatrix.h"   //�����
#include "..\Math\LibMyMath.h"   //������ѧ��


/*
 * Name										: CFFRELS_init
 * Description						: ��ʹ�ñ�ʶ֮ǰ��Ҫ���øú�����ʼ������ָ��3����Ҫ�ı�����ע�������ʶ�ĳ�ֵû�����û�ָ��
 * Entry                  : CFFRELS_T�Ľṹ��ָ�룬nf1��������������Ľ�����ng�Ľ����������ӳ�d
 * Return                 : void
 * Author									: lynx 84693469@qq.com.
 *
 * History
 * ----------------------
 * Rev										: 0.00
 * Date										: 06/14/2013
 *
 * create.
 * ----------------------
 */
void CFFRELS_init(CFFRELS_T* relsIn, int na, int nb, int d, float am1, float am2, float am3)
{
	int RELS_i = 0;  //ѭ���õı���
	float RELS_tmp_M1[CFFRELS_ML_A][CFFRELS_ML_A];  //���ھ����������ʱ���� ����
	float RELS_tmp_VT1[1][CFFRELS_ML_A];  //���ھ����������ʱ���� ת������
	float RELS_tmp_VT2[1][CFFRELS_ML_A];  //���ھ����������ʱ���� ת������
	
	if(na<=0 && nb<=0 && d<=0 && am1<=0 && am2<=0 && am3<=0){
		relsIn->NA = 2;  //���δ���������ؼ�������ʹ��Ĭ�ϲ���
		relsIn->NB = 1;
		relsIn->D = 3;
		matrix_init0((float*)relsIn->AM, 1, CFFRELS_ML_A);  //��ʼ��Ϊ0
		relsIn->AM[0][0] = 1.0;
		relsIn->AM[0][1] = -1.6;
		relsIn->AM[0][2] = 0.66;
		relsIn->NAM = 2;
	}else{
		relsIn->NA = na;  //���ṹ�帳ֵ
		relsIn->NB = nb;
		relsIn->D = d;
		matrix_init0((float*)relsIn->AM, 1, CFFRELS_ML_A);  //��ʼ��Ϊ0
		relsIn->AM[0][0] = am1;
		relsIn->AM[0][1] = am2;
		relsIn->AM[0][2] = am3;
		relsIn->NAM = 2;
	}
	
	//����һ��ѳ��Ȳ���
	relsIn->NF = relsIn->NB+relsIn->D;
	relsIn->NG = relsIn->NA;
	relsIn->NF1 = relsIn->NF-1;  //���˻��ּ��������һ������
	relsIn->NA0=2*relsIn->NA-relsIn->NAM-relsIn->NB-1;  //na0=2*na-nam-nb-1; %�۲�����ͽ״�
	
	//����ML
	relsIn->ML = relsIn->NF1+1+relsIn->NG+1;
	
	//����A0
	matrix_init0((float*)relsIn->A0, 1, CFFRELS_ML_A);  //��ʼ��Ϊ0
	matrix_init0((float*)RELS_tmp_VT1, 1, CFFRELS_ML_A);  //��ʼ��Ϊ0
	matrix_init0((float*)RELS_tmp_VT2, 1, CFFRELS_ML_A);  //��ʼ��Ϊ0
	relsIn->A0[0][0] = 1;   //��A0����ֵ
	RELS_tmp_VT1[0][0] = 1;   //[1 0.3-i*0.1]
	RELS_tmp_VT1[0][1] = 0.3; 
	for(RELS_i=0;RELS_i<relsIn->NA0;RELS_i++){
		RELS_tmp_VT1[0][1] -= 0.1;   //A0=conv(A0,[1 0.3-i*0.1]);%���ɹ۲���
		fconv((float*)relsIn->A0, 1+RELS_i, (float*)RELS_tmp_VT1, 2, (float*)RELS_tmp_VT2);
		matrix_copy((float*)RELS_tmp_VT2, 1, CFFRELS_ML_A, (float*)relsIn->A0);
		matrix_init0((float*)RELS_tmp_VT2, 1, CFFRELS_ML_A);  //��ʼ��Ϊ0
	}
	matrix_init0((float*)relsIn->AA, 1, CFFRELS_ML_A);  //��ʼ��Ϊ0
	fconv((float*)relsIn->A0, relsIn->NA0+1, (float*)relsIn->AM, relsIn->NAM+1, (float*)relsIn->AA);   //AA=conv(A0,Am); naa=na0+nam; %A0*Am
	relsIn->NAA = relsIn->NA0+relsIn->NAM;
	
	relsIn->NR = relsIn->NA0;
	
	
	//RELS��ʼ��
	//thetae_1=0.001*ones(na+nb+1+nc,1);%�ǳ�С���������˴�����Ϊ0��
	for(RELS_i=0;RELS_i<relsIn->ML;RELS_i++){   
		relsIn->thetae_1[RELS_i][0] = 0.001;  //0.001*1
	}
	//��Ϊ��thetae�ĸ�����ǰ�ˣ�����Ͷ�һ����ʼ����ֹthetae��0
	matrix_copy((float*)relsIn->thetae_1, relsIn->ML, 1, (float*)relsIn->thetae);
	//P=10^6*eye(na+nb+1+nc);  ��ʼ��P
	matrix_eye((float*)RELS_tmp_M1, relsIn->ML);
	matrix_multiply_k((float*)RELS_tmp_M1, 1000000.0, relsIn->ML, relsIn->ML, (float*)relsIn->P); 
}

/*
 * Name										: CFFRELS_initAsSteady
 * Description						: ����̬�Ĳ�����ʼ������ʹ�ñ�ʶ֮ǰ��Ҫ���øú�����ʼ������ָ��3����Ҫ�ı�����ע�������ʶ�ĳ�ֵû�����û�ָ��
 * Entry                  : CFFRELS_T�Ľṹ��ָ�룬nf1��������������Ľ�����ng�Ľ����������ӳ�d
 * Return                 : void
 * Author									: lynx 84693469@qq.com.
 *
 * History
 * ----------------------
 * Rev										: 0.00
 * Date										: 06/17/2013
 *
 * create.
 * ----------------------
 */
void CFFRELS_initAsSteady(CFFRELS_T* relsIn, int na, int nb, int d, float am1, float am2, float am3)
{
	int RELS_i = 0;  //ѭ���õı���
	
	const int MaxConstLength = 7;
	const float Steady_thetae_1[7][1] = {{0.00534},{0.0116},{0.0168},{0.00504},{2.456},{-4.337},{1.941}};
	const float Steady_P[7][7] =  {{6.761e-05,0.0001,0.0001,3.453e-05,0.01785,-0.03252,0.01467},{
    0.0001,0.0002,0.0003,7.7576e-05,0.03783,-0.06835,0.0305},{
    0.0001,0.0003,0.0004,0.0001,0.0523,-0.0939,0.04158},{
    3.4532e-05,7.758e-05,0.0001,5.880e-05,0.01454,-0.02524,0.01070},{
    0.01785,0.0378,0.05234,0.01454,7.3881,-13.34,5.9559},{
    -0.03252,-0.06835,-0.09392,-0.02524,-13.34,24.26,-10.92},{
    0.01467,0.0305,0.04158,0.01070,5.9559,-10.92,4.964}};
	
	
	CFFRELS_init(relsIn, na, nb, d, am1, am2, am3);  //Ϊ�˼���������룬��������ʼ��
	
	
	//RELS��ʼ��
	//���ո�����ֵ��ʼ��
	for(RELS_i=0;RELS_i<relsIn->ML && RELS_i<MaxConstLength;RELS_i++){   
		relsIn->thetae_1[RELS_i][0] = Steady_thetae_1[RELS_i][0];  //��Ԥ���趨��ֵ��
	}
	//��Ϊ��thetae�ĸ�����ǰ�ˣ�����Ͷ�һ����ʼ����ֹthetae��0
	matrix_copy((float*)relsIn->thetae_1, relsIn->ML, 1, (float*)relsIn->thetae);
	//���ո�����ֵ��ʼ��
	if(relsIn->ML > MaxConstLength){
		matrix_copy((float*)Steady_P, MaxConstLength, MaxConstLength, (float*)relsIn->P);
	}else{
		matrix_copy((float*)Steady_P, relsIn->ML, relsIn->ML, (float*)relsIn->P);
	}
	
	//������������һ����ʼ�Ŀ�����
	CFFRELS_ClcFGR(relsIn);
}


/*
 * Name										: CFFRELS_Update
 * Description						: �������������С���˱�ʶ�������Ǳջ���ϵͳ��ʶ������Ķ��Ǿ����˲��Ժ������
 * Entry                  : CFFRELS_T�Ľṹ��ָ�룬���˲�������ʱ�����У�����Ϊ0Խ��Խ��ȥ�������˲��������ʱ�����У�����Ϊ0Խ��Խ��ȥ�����������ӣ�0.9-1��
 * Return                 : void
 * Author									: lynx 84693469@qq.com.
 *
 * History
 * ----------------------
 * Rev										: 0.00
 * Date										: 06/14/2013
 *
 * create.
 * ----------------------
 * Rev										: 0.00
 * Date										: 06/17/2013
 *
 * ��FGR�����������
 * ----------------------
 */
void CFFRELS_Update(CFFRELS_T* relsIn, float CDataYFK[], float CDataUFK[], float lambda)
{
	//�����ȶ��������������ַ������ʱֻ�������Ƶķ�Χ
	float RELS_tmp_V1[CFFRELS_ML_A][1];  //���ھ����������ʱ���� ����
	float RELS_tmp_VT1[1][CFFRELS_ML_A];  //���ھ����������ʱ���� ת������
	float RELS_tmp_VT2[1][CFFRELS_ML_A];  //���ھ����������ʱ���� ת������
	float RELS_tmp_M1[CFFRELS_ML_A][CFFRELS_ML_A];  //���ھ����������ʱ���� ����
	float RELS_tmp_M2[CFFRELS_ML_A][CFFRELS_ML_A];  //���ھ����������ʱ���� ����
	float RELS_tmp_M3[CFFRELS_ML_A][CFFRELS_ML_A];  //���ھ����������ʱ���� ����
	float RELS_tmp_M4[CFFRELS_ML_A][CFFRELS_ML_A];  //���ھ����������ʱ���� ����
	float RELS_tmp_UM1[1][1];  //���ھ����������ʱ���� ��λ����
	float RELS_tmp_U1 = 0;  //���ھ����������ʱ���� �м����
	int RELS_i = 0;  //ѭ���õı���
	
	//�淶������
	if(lambda>1){
		lambda = 1;
	}else if(lambda<0.9){
		lambda = 0.9;
	}
	
	//----------------------------------------------------------------------------------------
	//thetae_1=thetae(:,k);  ��䱻������Ƶ������Ϊ�˱�֤����ļ��У�������ֲ
	matrix_copy((float*)relsIn->thetae, relsIn->ML, 1, (float*)relsIn->thetae_1);
	//���ȹ���۲�������   phie=[ufk(d:d+nf1);yfk(d:d+ng)];   %����۲���ǳ�������F
	//ע������û�к��ƣ���Ϊ��D�Ĵ��ڸ�����
	for(RELS_i=0;RELS_i<(relsIn->NF1+1);RELS_i++){   //ufk(d:d+nf1)
		relsIn->phie[RELS_i][0] = CDataUFK[relsIn->D+RELS_i];  //����Ҫע����D���Ҫ����+1 D=1ʱ�ӹ�ȥ�ĵ�һ��Ҳ�������ڵĵڶ���Ҳ����[1]��ʼ
	}
	for(RELS_i=0;RELS_i<(relsIn->NG+1);RELS_i++){   //yfk(d:d+ng)
		relsIn->phie[relsIn->NF1+1+RELS_i][0] = CDataYFK[relsIn->D+RELS_i];  //����Ҫע����D���Ҫ����+1
	}
	//K=P*phie/(lambda+phie'*P*phie);
	matrix_transpose((float*)relsIn->phie, relsIn->ML, 1, (float*)RELS_tmp_VT1);  //phie'
	matrix_multiply((float*)RELS_tmp_VT1, (float*)relsIn->P, 1, relsIn->ML, relsIn->ML, (float*)RELS_tmp_VT2); //phie'*P
	matrix_multiply((float*)RELS_tmp_VT2, (float*)relsIn->phie, 1, relsIn->ML, 1, (float*)RELS_tmp_UM1); //phie'*P*phie
	RELS_tmp_U1 = 1.0/(lambda+RELS_tmp_UM1[0][0]);  ///(lambda+phie'*P*phie)
	matrix_multiply((float*)relsIn->P, (float*)relsIn->phie, relsIn->ML, relsIn->ML, 1, (float*)RELS_tmp_V1); //P*phie
	matrix_multiply_k((float*)RELS_tmp_V1, RELS_tmp_U1, relsIn->ML, 1, (float*)relsIn->K);  //K=P*phie/(lambda+phie'*P*phie);
	//thetae(:,k)=thetae_1+K*(y(k)-phie'*thetae_1);
	matrix_multiply((float*)RELS_tmp_VT1, (float*)relsIn->thetae_1, 1, relsIn->ML, 1, (float*)RELS_tmp_UM1); //phie'*thetae_1  ǰ���Ѿ����phie'�ˣ�����ֱ����,ǰ��ע�Ᵽ��
	RELS_tmp_U1 = CDataYFK[0]-RELS_tmp_UM1[0][0];  //y(k)-phie'*thetae_1
	matrix_multiply_k((float*)relsIn->K, RELS_tmp_U1, relsIn->ML, 1, (float*)RELS_tmp_V1);  //K*(y(k)-phie'*thetae_1)
	matrix_addition((float*)relsIn->thetae_1, (float*)RELS_tmp_V1, relsIn->ML, 1, (float*)relsIn->thetae);  //thetae(:,k)=thetae_1+K*(y(k)-phie'*thetae_1);
	//P=(eye(nf+ng+2)-K*phie')*P/lambda;
	matrix_multiply((float*)relsIn->K, (float*)RELS_tmp_VT1, relsIn->ML, 1, relsIn->ML, (float*)RELS_tmp_M1); //K*phie' ǰ���Ѿ����phie'�ˣ�����ֱ����,ǰ��ע�Ᵽ��
	matrix_eye((float*)RELS_tmp_M2, relsIn->ML);  //eye(na+nb+1+nc)
	matrix_minus((float*)RELS_tmp_M2, (float*)RELS_tmp_M1, relsIn->ML, relsIn->ML, (float*)RELS_tmp_M3);   //(eye(na+nb+1+nc)-K*phie')
	matrix_multiply((float*)RELS_tmp_M3, (float*)relsIn->P, relsIn->ML, relsIn->ML, relsIn->ML, (float*)RELS_tmp_M4);  //(eye(na+nb+1+nc)-K*phie')*P
	matrix_multiply_k((float*)RELS_tmp_M4, 1.0/lambda, relsIn->ML, relsIn->ML, (float*)relsIn->P);  //P=(eye(nf+ng+2)-K*phie')*P/lambda;

//		%������С���˷�
//     phie=[ufk(d:d+nf1);yfk(d:d+ng)];   %����۲���ǳ�������F
//     K=P*phie/(lambda+phie'*P*phie);
//     thetae(:,k)=thetae_1+K*(y(k)-phie'*thetae_1);
//     P=(eye(nf1+ng+2)-K*phie')*P/lambda;   %����ĳ���Ҳ����

// 	//����۲�õ���be0 Fe Ge R
// 	relsIn->BE0 = relsIn->thetae[0][0];   //be0=thetae(1,k); 
// 	matrix_multiply_k((float*)relsIn->thetae, 1.0/relsIn->BE0, 1, relsIn->ML, (float*)RELS_tmp_V1);   // thetaeb(:,k)=thetae(:,k)/be0;
// 	for(RELS_i=0;RELS_i<(relsIn->NF1+1);RELS_i++){   //F1e=thetaeb(1:nf1+1,k)';
// 		relsIn->F1E[0][RELS_i] = RELS_tmp_V1[RELS_i][0];  
// 	}
// 	for(RELS_i=0;RELS_i<relsIn->NG+1;RELS_i++){   //Ge=thetaeb(nf1+2:nf1+ng+2,k)'; 
// 		relsIn->GE[0][RELS_i] = RELS_tmp_V1[relsIn->NF1+1+RELS_i][0];  
// 	}
// 	
// 	//Fe=conv(F1e,deltaF);  %�����������F �ȡ�F
// 	matrix_init0((float*)RELS_tmp_VT2, 1, CFFRELS_ML_A);  //��ʼ��Ϊ0
// 	RELS_tmp_VT2[0][0] = 1;   //[1 -1]
// 	RELS_tmp_VT2[0][1] = -1;
// 	matrix_init0((float*)relsIn->FE, 1, CFFRELS_ML_A);  //��ʼ��Ϊ0
// 	fconv((float*)relsIn->F1E, relsIn->NF1+1, (float*)RELS_tmp_VT2, 2, (float*)relsIn->FE);   //Fe=conv(F1e,deltaF);
// 	
// 	
// 	matrix_init0((float*)relsIn->R, 1, CFFRELS_ML_A);  //��ʼ��Ϊ0
// 	//Bm1=sum(Am)/be0; %Bm'
// 	RELS_tmp_U1 = 0;
// 	for(RELS_i=0;RELS_i<relsIn->NAM+1;RELS_i++){   //sum(Am)
// 		RELS_tmp_U1 += relsIn->AM[0][RELS_i];
// 	}
// 	
// 	
// 	//���ֹ�ʵİ취���ܻ���������һ���������ϵͳ��ɢ�Ļ�����û���κΰ취��
// 	if(relsIn->BE0 < 0.00001 && relsIn->BE0 > -0.00001){ //��������ǿ��Խ���жϣ�������������ȡ�ã���̫���ˣ���Ϊb0�����ͺ�С
// 		relsIn->BE0 = 0.00001;
// 	}
// //�������Ӧ������������Ч�����ɺܲ�
// // 	if((int)(relsIn->BE0*1000) == 0){ //��������ǿ��Խ���жϣ�������������ȡ�ã���̫���ˣ���Ϊb0�����ͺ�С
// // 		relsIn->BE0 = 0.00001;
// // 	}
// 	RELS_tmp_U1 /= relsIn->BE0;  //Bm1=sum(Am)/be0;
// 	
// 	//R=Bm1*A0;
// 	matrix_multiply_k((float*)relsIn->A0, RELS_tmp_U1, 1, CFFRELS_ML_A, (float*)relsIn->R);


	//һ�е�һ�м���FGR�ļ��㶼���ƶ���������
	CFFRELS_ClcFGR(relsIn);
}



/*
 * Name										: CFFRELS_ClcFGR
 * Description						: ���ݱ�ʶ�õ���theta������FGR�Ĳ���
 * Entry                  : CFFRELS_T�Ľṹ��ָ��
 * Return                 : void
 * Author									: lynx 84693469@qq.com.
 *
 * History
 * ----------------------
 * Rev										: 0.00
 * Date										: 06/17/2013
 *
 * create.��update��������ȡ����
 * ----------------------
 */
void CFFRELS_ClcFGR(CFFRELS_T* relsIn)
{
	//�����ȶ��������������ַ������ʱֻ�������Ƶķ�Χ
	float RELS_tmp_V1[CFFRELS_ML_A][1];  //���ھ����������ʱ���� ����
	float RELS_tmp_VT2[1][CFFRELS_ML_A];  //���ھ����������ʱ���� ת������
	float RELS_tmp_U1 = 0;  //���ھ����������ʱ���� �м����
	int RELS_i = 0;  //ѭ���õı���

	//����۲�õ���be0 Fe Ge R
	relsIn->BE0 = relsIn->thetae[0][0];   //be0=thetae(1,k); 
	matrix_multiply_k((float*)relsIn->thetae, 1.0/relsIn->BE0, 1, relsIn->ML, (float*)RELS_tmp_V1);   // thetaeb(:,k)=thetae(:,k)/be0;
	for(RELS_i=0;RELS_i<(relsIn->NF1+1);RELS_i++){   //F1e=thetaeb(1:nf1+1,k)';
		relsIn->F1E[0][RELS_i] = RELS_tmp_V1[RELS_i][0];  
	}
	for(RELS_i=0;RELS_i<relsIn->NG+1;RELS_i++){   //Ge=thetaeb(nf1+2:nf1+ng+2,k)'; 
		relsIn->GE[0][RELS_i] = RELS_tmp_V1[relsIn->NF1+1+RELS_i][0];  
	}
	
	//Fe=conv(F1e,deltaF);  %�����������F �ȡ�F
	matrix_init0((float*)RELS_tmp_VT2, 1, CFFRELS_ML_A);  //��ʼ��Ϊ0
	RELS_tmp_VT2[0][0] = 1;   //[1 -1]
	RELS_tmp_VT2[0][1] = -1;
	matrix_init0((float*)relsIn->FE, 1, CFFRELS_ML_A);  //��ʼ��Ϊ0
	fconv((float*)relsIn->F1E, relsIn->NF1+1, (float*)RELS_tmp_VT2, 2, (float*)relsIn->FE);   //Fe=conv(F1e,deltaF);
	
	
	matrix_init0((float*)relsIn->R, 1, CFFRELS_ML_A);  //��ʼ��Ϊ0
	//Bm1=sum(Am)/be0; %Bm'
	RELS_tmp_U1 = 0;
	for(RELS_i=0;RELS_i<relsIn->NAM+1;RELS_i++){   //sum(Am)
		RELS_tmp_U1 += relsIn->AM[0][RELS_i];
	}
	
	
	//���ֹ�ʵİ취���ܻ���������һ���������ϵͳ��ɢ�Ļ�����û���κΰ취��
	if(relsIn->BE0 < 0.00001 && relsIn->BE0 > -0.00001){ //��������ǿ��Խ���жϣ�������������ȡ�ã���̫���ˣ���Ϊb0�����ͺ�С
		relsIn->BE0 = 0.00001;
	}
//�������Ӧ������������Ч�����ɺܲ�
// 	if((int)(relsIn->BE0*1000) == 0){ //��������ǿ��Խ���жϣ�������������ȡ�ã���̫���ˣ���Ϊb0�����ͺ�С
// 		relsIn->BE0 = 0.00001;
// 	}
	RELS_tmp_U1 /= relsIn->BE0;  //Bm1=sum(Am)/be0;
	
	//R=Bm1*A0;
	matrix_multiply_k((float*)relsIn->A0, RELS_tmp_U1, 1, CFFRELS_ML_A, (float*)relsIn->R);
}

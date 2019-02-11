/****
 * @file   : EscPosExecutor.h
 * @brief  : esc posָ���װ��
 *
 * @remarks:
 ****/
#pragma once
class CEscPosExecutor
{
public:
	enum EAligmentStyle
	{
		ELeft,
		EMid,
		ERight
	};

public:
	CEscPosExecutor(void);
	~CEscPosExecutor(void);

	/**
	 * @brief �����ӡ������������,��������������ã����أ�˫�����»��ߣ��ض���С���෴��ӡ�ȣ�
	 *
	 * @fn InitPrinter
	 * @param szCmd �������ɵ����
	 * @return int �����������
	 */
	int InitPrinter(char *szCmd );

	/**
	 * @brief ѡ����߿հ�,�趨������ǰ��ӡ�����������޴�ӡ���ݡ�
	 * ������ó����ܴ�ӡ�������趨��߽�Ϊ��ӡ���򣬵���������ʱ���ұ��ڳ�һ���ֿ�+�ּ��д��һ���ַ�
	 *
	 * @fn SetLeftMargin
	 * @param dValue ��߾೤��mmΪ��λ
	 * @param szCmd �������ɵ�����м���ܻ����0
	 * @return int �����������
	 */
	int SetLeftMargin(double dValue, char *szCmd);

	/**
	 * @brief ������Դ�ӡλ��,���÷���Ϊˮƽ����ǰ��ӡλ���ұ�
	 *
	 * @fn SetRelativePostion
	 * @param dValue �뵱ǰ��ӡλ�õ�ƫ����mmΪ��λ
	 * @param szCmd �������ɵ�����м���ܻ����0
	 * @return int �����������
	 */
	int SetRelativePostion(double dValue, char *szCmd);

	/**
	 * @brief ���þ��Դ�ӡλ��
	 *
	 * @fn SetAbsoluteposition
	 * @param dValue �뵱ǰ��ӡλ�õ�ƫ����mmΪ��λ
	 * @param szCmd �������ɵ�����м���ܻ����0
	 * @return int �����������
	 */
	int SetAbsoluteposition(double dValue, char *szCmd);


	/**
	 * @brief ���ñ�׼�о�
	 *
	 * @fn SetStandardLineDistance
	 * @param szCmd �������ɵ����
	 * @return int �����������
	 */
	int SetStandardLineDistance(char *szCmd);

	/**
	 * @brief �����о�
	 *
	 * @fn SetCustomLineDistance
	 * @param dValue ����������ָ߶ȣ��������С�ڵ���һ���ַ��߶ȣ�3mm���򰴴�ӡ�����д�������һ���ַ��߶�ʱ�������Ϊ���о���
	 * @param szCmd �������ɵ����
	 * @return int �����������
	 */
	int SetCustomLineDistance(double dValue, char *szCmd);

	/**
	 * @brief ѡ�����з�ʽ,�趨������ǰ��ӡ�����������޴�ӡ����,
	 * �趨��������趨���Դ�ӡ����Դ�ӡ��Ч���ݰ����з�ʽ��ӡ���趨�˾��Դ�ӡ�������з�ʽ��
	 * �����з�ʽ��Ч���ݰ����Դ�ӡλ�ô�ӡ������������Դ�ӡ��ϵ��˭����˭��Ч��
	 * ע����Դ�ӡֻ�ǵ�ǰ����Ч�����з�ʽ���趨�󲻸ı���һֱ��Ч��
	 *
	 * @fn SetAlignment
	 * @param eAligmentStyle
	 * @param szCmd �������ɵ�����м���ܻ����0
	 * @return int �����������
	 */
	int SetAlignment(EAligmentStyle eAligmentStyle, char *szCmd );

	/**
	 * @brief ѡ���ַ���С, 1�ܴ�ֱ������8��1��ˮƽ������8,
	 * ������������ַ���Ч, ���糬���涨�ķ�Χ�����������,
	 * ��˳ʱ����ת90��ģʽʱ����ˮƽ�ʹ�ֱ�����ϵ������,
	 * �����һ�����в�ͬ�����߶ȵ����壬�������ַ��ڵ��߳�һֱ�����С�
	 * ESC ������������򿪺͹ر�˫�����˫����ģʽ�����ǣ������������ò���Ч
	 *
	 * @fn ZoomFont
	 * @param nMultiWidth �ַ�������ű���
	 * @param nMultiHeight �ַ��߶����ű���
	 * @param szCmd �������ɵ�����м���ܻ����0
	 * @return int �����������
	 */
	int ZoomFont(int nMultiWidth, int nMultiHeight, char *szCmd);

	/**
	 * @brief ���ݵ�ǰ�е��м���ӡ��ӡ�����������ݲ���ֽһ��,�޴�ӡ����ʱֻ��ֽ1��
	 *
	 * @fn PrintAndNewLine
	 * @param szCmd
	 * @return int
	 */
	int PrintAndNewLine(char *szCmd, bool bLineFeed);

private:
	static const double m_dStep;
};

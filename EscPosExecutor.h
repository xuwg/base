/****
 * @file   : EscPosExecutor.h
 * @brief  : esc pos指令封装类
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
	 * @brief 清除打印缓冲区的数据,清除各项命令设置（加重，双击，下划线，特定大小，相反打印等）
	 *
	 * @fn InitPrinter
	 * @param szCmd 返回生成的命令串
	 * @return int 返回命令串长度
	 */
	int InitPrinter(char *szCmd );

	/**
	 * @brief 选择左边空白,设定该命令前打印缓冲区必须无打印数据。
	 * 如果设置超过能打印区域，则设定左边界为打印区域，当有数据来时则靠右边腾出一个字宽+字间距写下一个字符
	 *
	 * @fn SetLeftMargin
	 * @param dValue 左边距长度mm为单位
	 * @param szCmd 返回生成的命令串中间可能会包含0
	 * @return int 返回命令串长度
	 */
	int SetLeftMargin(double dValue, char *szCmd);

	/**
	 * @brief 设置相对打印位置,设置方向为水平方向当前打印位置右边
	 *
	 * @fn SetRelativePostion
	 * @param dValue 与当前打印位置的偏移量mm为单位
	 * @param szCmd 返回生成的命令串中间可能会包含0
	 * @return int 返回命令串长度
	 */
	int SetRelativePostion(double dValue, char *szCmd);

	/**
	 * @brief 设置绝对打印位置
	 *
	 * @fn SetAbsoluteposition
	 * @param dValue 与当前打印位置的偏移量mm为单位
	 * @param szCmd 返回生成的命令串中间可能会包含0
	 * @return int 返回命令串长度
	 */
	int SetAbsoluteposition(double dValue, char *szCmd);


	/**
	 * @brief 设置标准行距
	 *
	 * @fn SetStandardLineDistance
	 * @param szCmd 返回生成的命令串
	 * @return int 返回命令串长度
	 */
	int SetStandardLineDistance(char *szCmd);

	/**
	 * @brief 设置行距
	 *
	 * @fn SetCustomLineDistance
	 * @param dValue 所设参数带字高度，如果设置小于等于一倍字符高度（3mm）则按打印不空行处理，大于一倍字符高度时多出的作为空行距离
	 * @param szCmd 返回生成的命令串
	 * @return int 返回命令串长度
	 */
	int SetCustomLineDistance(double dValue, char *szCmd);

	/**
	 * @brief 选择排列方式,设定该命令前打印缓冲区必须无打印数据,
	 * 设定该命令后设定绝对打印则绝对打印无效数据按排列方式打印。设定了绝对打印再设排列方式，
	 * 则排列方式无效数据按绝对打印位置打印。该命令与绝对打印关系：谁先设谁有效。
	 * 注意绝对打印只是当前行有效，排列方式是设定后不改变则一直有效。
	 *
	 * @fn SetAlignment
	 * @param eAligmentStyle
	 * @param szCmd 返回生成的命令串中间可能会包含0
	 * @return int 返回命令串长度
	 */
	int SetAlignment(EAligmentStyle eAligmentStyle, char *szCmd );

	/**
	 * @brief 选择字符大小, 1≤垂直倍数≤8，1≤水平倍数≤8,
	 * 该命令对所有字符有效, 假如超出规定的范围，该命令忽略,
	 * 当顺时针旋转90度模式时，那水平和垂直方向关系反过来,
	 * 如果在一行里有不同倍数高度的字体，则所有字符在底线成一直线排列。
	 * ESC ！命令能另外打开和关闭双倍宽和双倍高模式，但是，最后的命令设置才有效
	 *
	 * @fn ZoomFont
	 * @param nMultiWidth 字符宽度缩放倍数
	 * @param nMultiHeight 字符高度缩放倍数
	 * @param szCmd 返回生成的命令串中间可能会包含0
	 * @return int 返回命令串长度
	 */
	int ZoomFont(int nMultiWidth, int nMultiHeight, char *szCmd);

	/**
	 * @brief 根据当前行的行间距打印打印缓冲区的数据并走纸一行,无打印数据时只走纸1行
	 *
	 * @fn PrintAndNewLine
	 * @param szCmd
	 * @return int
	 */
	int PrintAndNewLine(char *szCmd, bool bLineFeed);

private:
	static const double m_dStep;
};

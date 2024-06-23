/* vim: set tabstop=4 : */
#ifndef __terark_unzip_prefer_h__
#define __terark_unzip_prefer_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif


namespace terark { namespace prefix_zip {

/**
 @brief ������ͬ key ��Ӧ�Ĳ�ͬ value ��Ŀ��С�Ĳ�ͬ��ʹ�ò�ͬ���㷨

 ���ַ�����ѹʱ��Ҫͨ��������ȷ��ǰ׺���ַ����ߴ�
 - ���ظ���Ŀ��Сʱ��ʹ�������������죬
 - �ϴ�ʱ��ʹ�� near bin search �Ͽ�
 - ���ܴ�ʱ��ʹ�ô�ͳ�� bin search ����
 */
enum unzip_prefer
{
	uzp_large_dup,
	uzp_small_dup,
	uzp_middle_dup
};

} } // namespace terark::prefix_zip


#endif // __terark_unzip_prefer_h__


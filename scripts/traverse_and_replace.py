import os
import sys


def replace_crlf_in_file(file_path):
    """
    替换指定文件中的 \r\n 为 \n
    """
    try:
        with open(file_path, 'r', encoding='utf-8', errors='ignore') as file:
            content = file.read()

        # 替换所有的 \r\n 为 \n
        content = content.replace('\r\n', '\n')

        # 将修改后的内容写回文件
        with open(file_path, 'w', encoding='utf-8', errors='ignore') as file:
            file.write(content)
        print(f"Successfully processed: {file_path}")

    except Exception as e:
        print(f"Error processing {file_path}: {e}")


def traverse_and_replace(directories):
    """
    遍历多个文件夹并递归替换其中目标文件的 \r\n 为 \n
    """
    for directory in directories:
        if not os.path.isdir(directory):
            print(f"Warning: {directory} 不是有效的文件夹路径，跳过.")
            continue

        # 遍历文件夹及子文件夹中的所有文件
        for root, dirs, files in os.walk(directory):  # os.walk 会递归遍历子目录
            for file in files:
                # 只处理目标类型的文件
                if file.endswith(('.cpp', '.h', '.txt', '.md', '.sh', '.proto')):
                    file_path = os.path.join(root, file)
                    replace_crlf_in_file(file_path)


if __name__ == "__main__":
    # 文件夹路径
    directories = [
        "../sample-datacenter",
        "../sample-datacenter",
        "../sample-datacenter-test",
        "../sample-db",
        "../sample-db-test",
        "../sample-dynamic-lib",
        "../sample-main",
        "../sample-net",
        "../sample-net-test",
        "../sample-pb",
        "../sample-tools",
        "../sample-tools-test",
    ]

    # 调用遍历并替换函数
    traverse_and_replace(directories)

#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>

MODULE_DESCRIPTION("DinterR: Data interaction Reporting LKM");
MODULE_AUTHOR("Gabe West <gawest@noctrl.edu>");
MODULE_LICENSE("GPL v2");

static char* dinterr_filename = NULL;

module_param(dinterr_filename, charp, 0);
MODULE_PARM_DESC(dinterr_filename, "Path to file for reporting");

static const struct file_operations fops = {
    .owner = THIS_MODULE
};

const char* get_dinterr_filename(void)
{
    return(dinterr_filename);
}
EXPORT_SYMBOL(get_dinterr_filename);

static int __init dinterr_init(void)
{
        pr_info("init\n");
        if (dinterr_filename)
            pr_info("DinterR File: %s\n", dinterr_filename);
        return 0;
}

static void __exit dinterr_exit(void)
{
        pr_info("%s\n", THIS_MODULE->name);
        pr_info("exit\n");
}

module_init(dinterr_init);
module_exit(dinterr_exit);

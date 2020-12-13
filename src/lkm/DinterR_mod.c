#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>

MODULE_DESCRIPTION("DinterR: Data interaction Reporting LKM");
MODULE_AUTHOR("Gabe West <gawest@noctrl.edu>");
MODULE_LICENSE("GPL v2");

static const struct file_operations fops = {
    .owner = THIS_MODULE
};

static int dinterr_init(void)
{
        pr_info("init\n");
        return 0;
}

static void dinterr_exit(void)
{
        pr_info("exit\n");
}

module_init(dinterr_init);
module_exit(dinterr_exit);

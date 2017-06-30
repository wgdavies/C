/**
 * $Author: $
 * $Date: $
 * $Revision: $
 *
 * HDB - Hadoop queue creation database
 **/

#include <stdio.h>
#include <stdlib.h>
#include <mxml.h>

int
main(void)
{
    FILE *fp;
    mxml_node_t *tree;
    
    mxml_node_t *comment;
    const char *comment_block = "/opt/mapr/hadoop/hadoop/conf/pools.xml ; Pool for <pool name> jobs";
    comment = mxmlNewElement(MXML_NO_PARENT, comment_block);

    mxml_node_t *xml;

    mxml_node_t *pool;
    const char *pool_name = "name";
    const char *pool_name_string = "pool name";

    mxml_node_t *pool_sm;
    const char *pool_sm_name = "schedulingMode";
    const char *pool_sm_val = "fair";

    mxml_node_t *pool_mm;
    const char *pool_mm_name = "minMaps";
    const char *pool_mm_val = "780";

    mxml_node_t *pool_mr;
    const char *pool_mr_name = "minReduces";
    const char *pool_mr_val = "540";

    mxml_node_t *pool_mrj;
    const char *pool_mrj_name = "maxRunningJobs";
    const char *pool_mrj_val = "60";

    mxml_node_t *pool_mspt;
    const char *pool_mspt_name = "minSharePreemptionTimeout";
    const char *pool_mspt_val = "300";

    mxml_node_t *pool_wt;
    const char *pool_wt_name = "weight";
    const char *pool_wt_val = "1.0";

    mxml_node_t *user;
    const char *user_name = "name";
    const char *user_name_string = "user name";

    mxml_node_t *user_mrj;
    const char *user_mrj_name = "maxRunningJobs";
    const char *user_jobs = "20";

    xml = mxmlNewXML("1.0");
    pool = mxmlNewElement(xml, "pool");
    mxmlElementSetAttr(pool, pool_name, pool_name_string);

    pool_sm = mxmlNewElement(pool, pool_sm_name);
    mxmlNewText(pool_sm, 0, pool_sm_val);

    pool_mm = mxmlNewElement(pool, pool_mm_name);
    mxmlNewText(pool_mm, 0, pool_mm_val);

    pool_mr = mxmlNewElement(pool, pool_mr_name);
    mxmlNewText(pool_mr, 0, pool_mr_val);

    pool_mrj = mxmlNewElement(pool, pool_mrj_name);
    mxmlNewText(pool_mrj, 0, pool_mrj_val);

    pool_mspt = mxmlNewElement(pool, pool_mspt_name);
    mxmlNewText(pool_mspt, 0, pool_mspt_val);

    pool_wt = mxmlNewElement(pool, pool_wt_name);
    mxmlNewText(pool_wt, 0, pool_wt_val);

    user = mxmlNewElement(xml, "user");
    mxmlElementSetAttr(user, user_name, user_name_string);

    user_mrj = mxmlNewElement(user, user_mrj_name);
    mxmlNewText(user_mrj, 0, user_jobs);

    tree = xml;
    
    fp = fopen("hdb.xml", "w");
    mxmlSaveFile(tree, fp, MXML_NO_CALLBACK);
    fclose(fp);

    mxmlDelete(tree);

    return(0);
}

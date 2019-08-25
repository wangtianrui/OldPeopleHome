package com.oldpeoplehome.entity;

/**
 * Created By Jiangyuwei on 2019/8/21 15:06
 * Description:
 */
public class ChildParent {

    private long parentId;
    private long childId;
    private String relation;

    public ChildParent() {
    }

    public ChildParent(long parentId, long childId, String relation) {
        this.parentId = parentId;
        this.childId = childId;
        this.relation = relation;
    }

    @Override
    public String toString() {
        return "ChildParent{" +
                "parentId=" + parentId +
                ", childId=" + childId +
                ", relation='" + relation + '\'' +
                '}';
    }
}

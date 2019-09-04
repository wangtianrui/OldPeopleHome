package com.oldpeoplehome.service;

import com.oldpeoplehome.BaseTest;
import com.oldpeoplehome.dto.HeartRateFilter;
import com.oldpeoplehome.entity.HeartRate;
import org.junit.Test;
import org.springframework.beans.factory.annotation.Autowired;

/**
 * Created By Jiangyuwei on 2019/8/23 13:34
 * Description:
 */
public class HeartRateServiceTest extends BaseTest {

    @Autowired
    private HeartRateService heartRateService;

    @Test
    public void test(){
        HeartRate heartRate = new HeartRate(1, "2019-8-22 10:00:00", 20.3, 21,21);
        heartRateService.insert(heartRate);
//        HeartRateFilter heartRateFilter = new HeartRateFilter(1);
//        System.out.println(heartRateService.findByParent(heartRateFilter));
    }

}

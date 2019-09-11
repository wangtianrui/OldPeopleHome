package com.oldpeoplehome.utils;

import javax.servlet.*;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import java.io.IOException;

/**
 * Created By Jiangyuwei on 2019/9/9 16:27
 * Description:
 */
public class SimpleCORSFilter implements Filter {

    public void doFilter(ServletRequest req, ServletResponse resp, FilterChain chain) throws IOException, ServletException {
        HttpServletResponse response = (HttpServletResponse) resp;
        HttpServletRequest request=(HttpServletRequest)req;
        // 处理简单请求
        // 跨域请求默认不携带cookie,如果要携带cookie，需要设置下边2个响应头
        response.setHeader("Access-Control-Allow-Origin", request.getHeader("Origin"));// 必选，所有有效的跨域响应都必须包含这个请求头, 没有的话会导致跨域请求失败
        response.setHeader("Access-Control-Allow-Credentials", "true");//可选，此处设置为true,对应前端 xhr.withCredentials = true;
        //处理非简单请求
        //  非简单请求：浏览器会发送两个请求, 第一个请求(成为预检请求)会像服务器确定是否接受这个跨域请求, 第二个才是真正的发出请求. 浏览器自动的处理这两个请求, 同时预检请求也是可以被缓存的, 而不用每次请求都需要发送预检请求.
        //  预检请求是在实际的请求发出前先向服务器确认是否能够处理这个请求. 服务器应该检查上边两个请求头的值, 来判断这个请求是否有效.
        response.setHeader("Access-Control-Allow-Methods", "POST,GET,OPTIONS,DELETE");// 必选
        response.setHeader("Access-Control-Allow-Headers",
                "Origin,No-Cache,X-Requested-With,If-Modified-Since," +
                        "Pragma,Last-Modified,Cache-Control,Expires,Content-Type,X-E4M-With,userId,token");

        response.setHeader("Access-Control-Max-Age", "0");// 可选,在每个请求前面都发送一个预检请求是很浪费资源的, 这个值允许你设置预检请求的缓存时间, 单位是秒.
        response.setHeader("XDomainRequestAllowed","1");
        chain.doFilter(req,resp);
    }

    public void init(FilterConfig filterConfig) {}

    public void destroy() {}

}



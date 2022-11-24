import 'package:flutter/material.dart';
import 'package:page_transition/page_transition.dart';

void nextScreen(context, page, transition) {
  // Navigator.push(
  //   context,
  //   PageTransition(
  //     opaque: true,
  //     duration: const Duration(milliseconds: 700),
  //     curve: Curves.elasticOut,
  //     type: transition,
  //     child: page,
  //   ),
  // );
  Navigator.push(
    context,
    MaterialPageRoute(
      builder: (context) => page,
    ),
  );
}

void nextScreenReplace(context, page, transition) {
  Navigator.pushReplacement(
    context,
    PageTransition(
      duration: const Duration(milliseconds: 300),
      curve: const ElasticInCurve(),
      type: transition,
      child: page,
    ),
  );
}